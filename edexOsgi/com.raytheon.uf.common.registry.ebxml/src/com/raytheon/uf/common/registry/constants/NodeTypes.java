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
package com.raytheon.uf.common.registry.constants;

/**
 * 
 * Node Types
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * 
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * Jul 31, 2012            bphillip     Initial creation
 * 4/9/2013     1802       bphillip    Moved into EBXML common plugin
 * 
 * </pre>
 * 
 * @author bphillip
 * @version 1.0
 */
public class NodeTypes {

    public static final String EMBEDDED_PATH = "urn:oasis:names:tc:ebxml-regrep:NodeType:EmbeddedPath";

    public static final String NON_UNIQUE_CODE = "urn:oasis:names:tc:ebxml-regrep:NodeType:NonUniqueCode";

    public static final String UNIQUE_CODE = "urn:oasis:names:tc:ebxml-regrep:NodeType:UniqueCode";
}
