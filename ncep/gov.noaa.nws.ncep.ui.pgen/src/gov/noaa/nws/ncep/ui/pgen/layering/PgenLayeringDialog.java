/*
 * gov.noaa.nws.ncep.ui.pgen.layering.PgenLayeringDialog
 * 
 * July 2009
 *
 * This code has been developed by the NCEP/SIB for use in the AWIPS2 system.
 */
package gov.noaa.nws.ncep.ui.pgen.layering;

import gov.noaa.nws.ncep.ui.pgen.PgenSession;
import gov.noaa.nws.ncep.ui.pgen.elements.Layer;
import gov.noaa.nws.ncep.ui.pgen.elements.Product;
import gov.noaa.nws.ncep.ui.pgen.rsc.PgenResource;

import java.awt.Color;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Dialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

/**
 * This class is the common dialog for PGEN layering control in PGEN.
 * 
 * <pre>
 * SOFTWARE HISTORY
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * 07/09  		#131		J. Wu		Initial creation. 
 * 
 * </pre>
 * 
 * @author jwu
 * @version 1.0
 * 
 */

public class PgenLayeringDialog extends Dialog {
    
    /**
     * Return object.
     */
    protected final Boolean returnValue = false;
    
	/**
     * Dialog shell.
     */
    protected Shell shell = null;
	
    protected Display display = null; 
           
	/**
     * PgenResource, active product, and active layer.
     */
    protected PgenResource	drawingLayer = null;
    protected Product		currentProduct = null;
    protected Layer			currentLayer = null;                 
	
    
    /**
     * Constructor.
     */
	public PgenLayeringDialog( Shell parentShell ) {
		
		super( parentShell );
				
        //drawingLayer = PgenSession.getInstance().getPgenResource();
        
	}

	/**
     * Open method used to display the layering dialog.
     * 
     * @return Return object (can be null).
     */
    public Object open() {

    	// Link to the drawing layer's active product & active layer;       
        drawingLayer = PgenSession.getInstance().getPgenResource();
        currentProduct = drawingLayer.getActiveProduct();
        currentLayer   = drawingLayer.getActiveLayer();

        // Reset the UNDO/REDO;   
        PgenSession.getInstance().disableUndoRedo();
        
        // Create the main shell; 
    	Shell parent = this.getParent();
        shell = new Shell( parent, SWT.DIALOG_TRIM | SWT.MODELESS );   	
        
        // Set the title of the dialog.
        setTitle();
		
        // Create the main layout for the shell.
        setLayout();
 
        // Set the default location.
        setDefaultLocation( parent );
        
        // Create and initialize all of the controls and layouts
        initializeComponents();
        
        
        // Pack and open
        shell.pack();
        shell.open();
                       
        /*
         *  Pops up a second window, such as the window to edit the layer's name.    
         *  Note: must be popped up before the event handling loop.
         */
        popupSecondDialog();
        
        // Event handling loop for this dialog
        display = parent.getDisplay();
        while ( !shell.isDisposed() ) {
            if ( !display.readAndDispatch()) {
                display.sleep();
            }
        }

        return returnValue;
    }
        
    /**
     *  Sets the title of the dialog.
     */
    public void setTitle() {    	
        shell.setText( "" );        
    }
    
    /**
     *  Creates the main layout for the shell.
     */
    public void setLayout() {
        
    	GridLayout mainLayout = new GridLayout( 1, true );
        
    	mainLayout.marginHeight = 1;
        mainLayout.marginWidth = 1;
        mainLayout.verticalSpacing = 2;
        mainLayout.horizontalSpacing = 1;
        
        shell.setLayout( mainLayout );
    }
    
    /**
     *  Set the default location.
     * @param parent
     */
    public void setDefaultLocation( Shell parent ) {
        Point pt = parent.getLocation();
        shell.setLocation( pt.x,  pt.y );
    }
    
    /**
     * Initialize the dialog components.
     */
    public void initializeComponents( boolean compact ) {   	          
    }
 
    /**
     * Initialize the dialog components.
     */
    public void initializeComponents() {
    	 initializeComponents( false );     
    }
   

    /**
     *  Check the dialog is opened or not
     */
   public boolean isOpen() {
        return ( shell != null && !shell.isDisposed() );
    }
 
 	
    /**
     *  Pops up a second dialog
     */    
    protected void popupSecondDialog() {       	
    }

    
    /**
     *  Set a button's color
     */    
    public void setButtonColor( Button btn, Color clr ) {
       	
		btn.setBackground( new org.eclipse.swt.graphics.Color( display, 
                           clr.getRed(), clr.getGreen(), clr.getBlue() ) );

    }
    
    
    /*
     *  Check the dialog is opened or not
     */
    public void close() {
        if ( shell != null && !shell.isDisposed() ) {
        	shell.dispose();
        }
    }   
   
}
