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
package com.raytheon.viz.aviation.monitor;

import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.raytheon.uf.common.dataplugin.binlightning.BinLightningRecord;
import com.raytheon.uf.common.dataquery.requests.RequestConstraint;
import com.raytheon.uf.viz.core.alerts.AlertMessage;
import com.raytheon.uf.viz.core.catalog.LayerProperty;
import com.raytheon.uf.viz.core.comm.Loader;
import com.raytheon.uf.viz.core.exception.VizException;
import com.raytheon.viz.aviation.observer.TafMonitorDlg;

/**
 * Watches incoming lightning data, and when it arrives, updates the cache and
 * then notifies the LtgMonitor to reapply the rules.
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * Sep 10, 2009            njensen     Initial creation
 * 
 * </pre>
 * 
 * @author njensen
 * @version 1.0
 */

public class LtgMonitorObserver extends MonitorObserver {

    public LtgMonitorObserver(TafMonitorDlg dlg) {
        super(dlg);
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * com.raytheon.viz.alerts.IAlertObserver#alertArrived(com.raytheon.viz.
     * alerts.IAlertObserver.AlertMessage[])
     */
    @Override
    public void alertArrived(Collection<AlertMessage> alertMessages) {
        for (AlertMessage alert : alertMessages) {
            BinLightningRecord rec = getRecord(alert.dataURI);
            LtgDataMgr.updateSiteData(new BinLightningRecord[] { rec });
            String msg = "Checking ltg for ALL";
            statusMessage(msg);
            for (TafSiteComp tsc : dialog.getTafSiteComps()) {
                tsc.fireMonitor("LtgMonitor");
            }
        }
    }

    private static BinLightningRecord getRecord(String dataUri) {
        BinLightningRecord rec = null;
        Map<String, RequestConstraint> map = new HashMap<String, RequestConstraint>();
        map.put("pluginName", new RequestConstraint("binlightning"));
        map.put("dataURI", new RequestConstraint(dataUri));
        LayerProperty lp = new LayerProperty();
        try {
            lp.setEntryQueryParameters(map);
            List<Object> objs = Loader.loadData(lp, "select", 10000);
            if (objs.size() > 0) {
                rec = (BinLightningRecord) objs.get(0);
            }
        } catch (VizException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return rec;
    }

}
