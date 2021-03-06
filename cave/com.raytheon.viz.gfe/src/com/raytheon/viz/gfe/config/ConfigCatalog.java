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
package com.raytheon.viz.gfe.config;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import jep.JepException;

import com.raytheon.uf.common.dataplugin.gfe.python.GfePyIncludeUtil;
import com.raytheon.uf.common.localization.ILocalizationFile;
import com.raytheon.uf.common.localization.IPathManager;
import com.raytheon.uf.common.localization.LocalizationContext.LocalizationType;
import com.raytheon.uf.common.localization.LocalizationFile;
import com.raytheon.uf.common.localization.PathManagerFactory;
import com.raytheon.uf.common.python.PyUtil;
import com.raytheon.uf.common.python.PythonScript;
import com.raytheon.uf.common.status.IUFStatusHandler;
import com.raytheon.uf.common.status.UFStatus;
import com.raytheon.uf.common.status.UFStatus.Priority;
import com.raytheon.uf.common.util.FileUtil;

/**
 * Catalog of gfe config files
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * Jan 21, 2010            randerso     Initial creation
 * Jul 08, 2014 3361       njensen      Consolidated code
 * Sep 29, 2014 2975       njensen      Only look up files in CAVE_STATIC
 * Feb 05, 2016 5242       dgilling     Remove calls to deprecated Localization APIs.
 * 
 * </pre>
 * 
 * @author randerso
 * @version 1.0
 */

public class ConfigCatalog {

    private static final transient IUFStatusHandler statusHandler = UFStatus
            .getHandler(ConfigCatalog.class);

    private static final String EXTENSION = ".py";

    protected List<String> preEvals;

    public ConfigCatalog() {
        super();
        preEvals = new ArrayList<String>();
        preEvals.add("HideConfigFile = False\n");
        preEvals.add("def checkHideConfigFile(): return bool(HideConfigFile)\n\n");

    }

    /**
     * Return true if HideConfigFile is set to true when the script is loaded.
     * 
     * @param name
     *            config file name
     * @return true if config file is hidden
     */
    public boolean isHidden(String name) {
        boolean rtnVal = false;

        LocalizationFile lf = getFile(name);
        if (lf == null || !lf.exists()) {
            throw new IllegalArgumentException("No such GFE config file: "
                    + name);
        } else {
            String configPath = GfePyIncludeUtil.getConfigIncludePath();
            String vtecPath = GfePyIncludeUtil.getVtecIncludePath();

            // Look for HideConfigFile = True in the file
            try (PythonScript pscript = new PythonScript(lf.getFile()
                    .getAbsolutePath(), PyUtil.buildJepIncludePath(configPath,
                    vtecPath), getClass().getClassLoader(), preEvals)) {
                Boolean scriptValue = (Boolean) pscript.execute(
                        "checkHideConfigFile", null);
                rtnVal = scriptValue.booleanValue();
            } catch (JepException e) {
                // Don't show file if Jep can't parse it
                rtnVal = true;
                statusHandler.handle(
                        Priority.PROBLEM,
                        "Error loading GFE config file: "
                                + e.getLocalizedMessage(), e);
            }
        }
        return rtnVal;
    }

    /**
     * Get all the localization files for this catalog.
     * 
     * @return the localization files for the procedures.
     */
    public ILocalizationFile[] getFiles() {
        ILocalizationFile[] procFiles = PathManagerFactory.getPathManager()
                .listStaticFiles(LocalizationType.CAVE_STATIC,
                        GfePyIncludeUtil.CONFIG, new String[] { EXTENSION },
                        false, true);
        return procFiles;
    }

    /**
     * Get the simple names of the procedures.
     * 
     * @return the simple names of the procedures.
     */
    public Collection<String> getNames() {
        ILocalizationFile[] procFiles = getFiles();
        Collection<String> result = scriptNames(procFiles);
        return result;
    }

    /**
     * @param scriptName
     *            The simple name of a procedure, i.e., "Align_Grids".
     * @return The localization file for the script
     */
    public LocalizationFile getFile(String scriptName) {
        String fname = GfePyIncludeUtil.CONFIG + IPathManager.SEPARATOR
                + scriptName + EXTENSION;
        LocalizationFile file = PathManagerFactory.getPathManager()
                .getStaticLocalizationFile(LocalizationType.CAVE_STATIC, fname);
        return file;
    }

    /**
     * Get the script names from an array of LocalizationFiles, with any leading
     * directories and trailing ".py"s removed.
     * 
     * @param scriptFiles
     *            the array of LocalizationFiles.
     * @return a Collection of simple script names with no duplicates.
     */
    protected Collection<String> scriptNames(ILocalizationFile[] scriptFiles) {
        Set<String> procs = new HashSet<String>();
        if (scriptFiles != null) {
            for (ILocalizationFile file : scriptFiles) {
                String fname = file.getPath();
                String[] fsplit = fname.split(FileUtil.fileSeparatorRegex);
                String script = fsplit[fsplit.length - 1].replaceAll("\\.py$",
                        "");
                procs.add(script);
            }
        }
        return procs;
    }

}
