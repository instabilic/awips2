/**
 * This software was developed and / or modified by Raytheon Company,
 * pursuant to Contract DG133W-05-CQ-1067 with the US Government.
 * 
 * U.S. EXPORT CONTROLLED TECHNICAL DATA
 * This software product contains export-restricted data whose
 * export/transfer/disclosure is restricted by U.S. law. Dissemination
 * to non-U.S. persons whether in the United States or abroad requires
 * an export license or other authorization.
 * 
 * Contractor Name:        Raytheon Company
 * Contractor Address:     6825 Pine Street, Suite 340
 *                         Mail Stop B8
 *                         Omaha, NE 68106
 *                         402.291.0100
 * 
 * See the AWIPS II Master Rights File ("Master Rights File.pdf") for
 * further licensing information.
 **/
package com.raytheon.edex.plugin.gfe.smartinit;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.raytheon.edex.plugin.gfe.server.IFPServer;
import com.raytheon.uf.common.dataplugin.gfe.db.objects.DatabaseID;
import com.raytheon.uf.common.localization.IPathManager;
import com.raytheon.uf.common.localization.LocalizationContext;
import com.raytheon.uf.common.localization.LocalizationContext.LocalizationLevel;
import com.raytheon.uf.common.localization.LocalizationContext.LocalizationType;
import com.raytheon.uf.common.localization.LocalizationUtil;
import com.raytheon.uf.common.localization.PathManagerFactory;
import com.raytheon.uf.edex.core.EDEXUtil;
import com.raytheon.uf.edex.core.EdexTimerBasedThread;

import jep.JepException;

/**
 * Service that runs smart inits
 * 
 * <pre>
 * SOFTWARE HISTORY
 * Date         Ticket#     Engineer    Description
 * ------------ ----------  ----------- --------------------------
 * Apr 22, 2008             njensen     Initial creation
 * Jul 17, 2009   #2590     njensen     Multiple site support
 * Jul 28, 2010   #6725     jdynina     Manual init support
 * Aug 27, 2010   #3688     wkwock      Find model class for a model
 * Aug 24, 2013   #1949     rjpeter     Updated start up logic
 * Jun 13, 2013   #2044     randerso    Refactored to use IFPServer, 
 *                                      added support to run init for all valid times
 * Mar 14, 2014   2726      rjpeter     Implement graceful shutdown.
 * Jul 12, 2016   5747      dgilling    Move edex_static to common_static.
 * Mar 30, 2017   5937      rjpeter     Use EdexTimerBasedThread logger.
 * </pre>
 * 
 * @author njensen
 */

public class SmartInitSrv extends EdexTimerBasedThread {
    private final Map<Long, SmartInitScript> cachedInterpreters = new HashMap<>();

    protected int pendingInitMinTimeMillis = 120000;

    protected int runningInitTimeOutMillis = 600000;

    public SmartInitSrv() {
    }

    @Override
    public String getThreadGroupName() {
        return "smartInitThreadPool";
    }

    @Override
    public void process() throws Exception {
        SmartInitRecord record = null;
        do {
            record = SmartInitTransactions.getSmartInitToRun(
                    pendingInitMinTimeMillis, runningInitTimeOutMillis);
            if (record != null) {
                runSmartInit(record);
            }
        } while ((record != null) && !EDEXUtil.isShuttingDown());
    }

    @Override
    public void dispose() {
        super.dispose();

        // Make sure OS resources are released at thread death
        SmartInitScript script = cachedInterpreters
                .remove(Thread.currentThread().getId());
        if (script != null) {
            script.dispose();
        }
    }

    public void runSmartInit(SmartInitRecord record) {
        try {
            SmartInitScript initScript = null;
            List<String> sitePathsAdded = new ArrayList<>(2);

            String init = record.getSmartInit();
            String dbName = record.getDbName()
                    + (record.isManual() ? ":1" : ":0");
            Date validTime = record.getId().getValidTime();
            if (SmartInitRecord.ALL_TIMES.equals(validTime)) {
                validTime = null;
            }

            DatabaseID db = new DatabaseID(record.getDbName());
            if (IFPServer.getActiveSites().contains(db.getSiteId())) {
                try {
                    long id = Thread.currentThread().getId();
                    initScript = cachedInterpreters.get(id);

                    if (initScript == null) {
                        initScript = SmartInitFactory.constructInit();
                        cachedInterpreters.put(id, initScript);
                    }

                    IPathManager pathMgr = PathManagerFactory.getPathManager();
                    LocalizationContext siteCtx = pathMgr.getContextForSite(
                            LocalizationType.COMMON_STATIC, db.getSiteId());
                    LocalizationContext baseCtx = pathMgr.getContext(
                            LocalizationType.COMMON_STATIC,
                            LocalizationLevel.BASE);

                    File file = pathMgr.getFile(siteCtx,
                            LocalizationUtil.join("gfe", "smartinit"));
                    if ((file != null) && file.exists()) {
                        initScript
                                .addSitePath(file.getPath(), pathMgr
                                        .getFile(baseCtx,
                                                LocalizationUtil.join("gfe",
                                                        "smartinit"))
                                        .getPath());
                        sitePathsAdded.add(file.getPath());
                    }
                    file = pathMgr.getFile(siteCtx,
                            LocalizationUtil.join("gfe", "config"));
                    if ((file != null) && file.exists()) {
                        initScript.addSitePath(file.getPath(),
                                pathMgr.getFile(baseCtx,
                                        LocalizationUtil.join("gfe", "config"))
                                        .getPath());
                        sitePathsAdded.add(file.getPath());
                    }

                    HashMap<String, Object> argMap = new HashMap<>();
                    argMap.put("dbName", dbName);
                    argMap.put("model", init);
                    argMap.put("validTime", validTime);

                    initScript.execute(argMap);
                } catch (Throwable e) {
                    logger.error(
                            "Error running smart init for " + record.getId(),
                            e);
                } finally {
                    try {
                        for (String path : sitePathsAdded) {
                            initScript.removeSitePath(path);
                        }
                    } catch (JepException e) {
                        logger.error(
                                "Error cleaning up smart init interpreter's sys.path",
                                e);
                    }
                }
            } else {
                logger.warn("Site " + db.getSiteId()
                        + " has been disabled. Smart init for "
                        + record.getDbName() + " will not be processed.");
            }
        } catch (Throwable t) {
            logger.error("Error in SmartInitSrv", t);
        }
        SmartInitTransactions.removeSmartInit(record);
    }

    @Override
    public void postStop() {
        SmartInitQueue.getQueue().fireSmartInit();
        super.postStop();
    }

    public int getPendingInitMinTimeMillis() {
        return pendingInitMinTimeMillis;
    }

    public void setPendingInitMinTimeMillis(int pendingInitMinTimeMillis) {
        this.pendingInitMinTimeMillis = pendingInitMinTimeMillis;
    }

    public int getRunningInitTimeOutMillis() {
        return runningInitTimeOutMillis;
    }

    public void setRunningInitTimeOutMillis(int runningInitTimeOutMillis) {
        this.runningInitTimeOutMillis = runningInitTimeOutMillis;
    }
}
