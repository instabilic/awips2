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
package com.raytheon.uf.viz.hprof;

import java.io.File;
import java.io.IOException;

import com.raytheon.hprof.HprofFile;

/**
 * 
 * Main class for exporting information from a cave heap dump. Parses command
 * line options and passes all the real work to various exporters.
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * 
 * Date          Ticket#  Engineer    Description
 * ------------- -------- ----------- --------------------------
 * Jan 08, 2014  2648     bsteffen    Initial doc
 * May 05, 2014  3093     bsteffen    Add some new exporters
 * May 20, 2014  3093     bsteffen    Add option to zero primitive arrays.
 * 
 * </pre>
 * 
 * @author bsteffen
 * @version 1.0
 */
public class CaveExporter {

    public static void main(String[] args) {
        if (args.length < 1) {
            System.out
                    .println("Provide some args, an hprof file is required, an output directory is optional.");
            System.exit(1);
        }
        int argIdx = 0;
        HprofFile hprof = null;
        try {
            if (args[argIdx].equals("-zero")) {
                System.out.println("Writing 0s...");
                hprof = new HprofFile(args[argIdx + 1], false);
                hprof.getHeapDump().zeroPrimitiveArrays();
                argIdx += 2;
            } else {
                hprof = new HprofFile(args[argIdx], true);
                argIdx += 1;
            }

        } catch (IOException e) {
            throw new IllegalArgumentException("Error opening hprof file: "
                    + args[argIdx], e);
        }

        File outputDir = null;
        if (args.length > 1) {
            outputDir = new File(args[argIdx++]);
        } else {
            outputDir = new File(".");
        }
        if (!outputDir.isDirectory()) {
            if (!outputDir.mkdirs()) {
                throw new IllegalArgumentException(
                        "Cannot make output directory: " + outputDir);
            }
        }
        DisplayedResourcesExporter displayPanes = new DisplayedResourcesExporter(
                hprof, outputDir);
        displayPanes.export();
        new RequestableResourceExporter(hprof, outputDir,
                displayPanes.getResources()).export();

        new D2DGridResourceExporter(hprof, outputDir).export();
        new GLInfoExporter(hprof, outputDir).export();
        new InputHandlerExporter(hprof, outputDir).export();
        new D2DProcedureDialogExporter(hprof, outputDir).export();
        new GFEResourceExporter(hprof, outputDir).export();
        new DisposingResourceExporter(hprof, outputDir).export();
        new UIRunnablesExporter(hprof, outputDir).export();
        new AlertMessageExporter(hprof, outputDir).export();
        new CacheObjectExporter(hprof, outputDir).export();
        new RadarMosaicExporter(hprof, outputDir).export();
    }
}
