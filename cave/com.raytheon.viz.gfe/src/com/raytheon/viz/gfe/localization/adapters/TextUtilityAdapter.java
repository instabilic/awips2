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
package com.raytheon.viz.gfe.localization.adapters;

import org.eclipse.jface.action.IMenuManager;

import com.raytheon.uf.viz.localization.perspective.adapter.LocalizationPerspectiveAdapter;
import com.raytheon.uf.viz.localization.perspective.view.FileTreeEntryData;
import com.raytheon.viz.gfe.localization.actions.NewAction;
import com.raytheon.viz.gfe.localization.util.AbstractScriptUtil;
import com.raytheon.viz.gfe.localization.util.TextUtilityUtil;

/**
 * GFE Text Utility adapter.
 * 
 * <pre>
 * 
 * SOFTWARE HISTORY
 * 
 * Date          Ticket#  Engineer  Description
 * ------------- -------- --------- --------------------------------------------
 * Oct 08, 2010           mpduff    Initial creation
 * Aug 11, 2016  5816     randerso  Moved to gfe.localization.adapters
 * 
 * </pre>
 * 
 * @author mpduff
 */

public class TextUtilityAdapter extends LocalizationPerspectiveAdapter {

    @Override
    public boolean addContextMenuItems(IMenuManager menuMgr,
            FileTreeEntryData[] selectedData) {
        AbstractScriptUtil util = new TextUtilityUtil();
        NewAction newAction = new NewAction(util);
        menuMgr.add(newAction);

        return true;
    }
}
