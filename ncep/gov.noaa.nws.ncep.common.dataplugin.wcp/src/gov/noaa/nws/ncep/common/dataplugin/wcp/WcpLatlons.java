/**
 * 
 * WcpLatLons
 * 
 * This class represents the lat/lons for a WCP record.   This contains the 
 * setters and getters for the grandchild table.
 * 
 * <pre>
 *      
 * SOFTWARE HISTORY
 *      
 * Date       	Ticket#		Engineer	Description
 * ------------	----------	-----------	--------------------------
 * 12Dec2008    37          F. J. Yen   Initial 
 * 17Apr2009	37			F. J. Yen	Redesigned table and refactored for TO10
 * 17May2010	37			F. J. Yen	Refactored to dataplugin for migration to to11dr11
 *       
 * </pre>
 * 
 * @author Fee Jing Yen, SIB
 * @version 1
 */

package gov.noaa.nws.ncep.common.dataplugin.wcp;

import java.io.Serializable;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import javax.persistence.Entity;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import com.raytheon.uf.common.serialization.ISerializableObject;
import com.raytheon.uf.common.serialization.annotations.DynamicSerialize;
import com.raytheon.uf.common.serialization.annotations.DynamicSerializeElement;
import gov.noaa.nws.ncep.common.tools.IDecoderConstantsN;

@Entity
@Table(name="wcp_latlons")
@XmlAccessorType(XmlAccessType.NONE)
@DynamicSerialize

public class WcpLatlons implements Serializable, ISerializableObject {

	private static final long serialVersionUID = 1L;
    private static final float RMISSD = IDecoderConstantsN.FLOAT_MISSING;
    // private static final float RMISSD = -99999.0f;
	
    @Id
    @GeneratedValue
    private Integer recordId = null;
    
	/** The wcp record this object belongs to **/
    @ManyToOne
    @JoinColumn(name="parentID", nullable=false)
	private WcpSevrln parentID;
	
	/** The latitude of the index-th position **/   
    //@DataURI(position=2)
    @XmlElement
    @DynamicSerializeElement
	private Float lat;

	/** The longitude of the index-th position **/
    //@DataURI(position=3)
    @XmlElement
    @DynamicSerializeElement
	private Float lon;
	
	/** index for the order of lat/lons */
    //@DataURI(position=4)
    @XmlElement
    @DynamicSerializeElement
	private Integer index;
				
    /**
     * No-Arg Constructor.
     */
	public WcpLatlons() {
            this.lat = RMISSD;
            this.lon = RMISSD;
            this.index = 0;
        }
	
	/**
	 * @return the serialVersionUID
	 */
	public static long getSerialVersionUID() {
		return serialVersionUID;
	}

	/**
	 * @return the parentID
	 */
	public WcpSevrln getParentID() {
		return parentID;
	}

	/**
	 * @param parentID the parentID to set
	 */
	public void setParentID(WcpSevrln parentID) {
		this.parentID = parentID;
	}
	
	/**
	 * @return The recordId.  If not set returns to null.
	 */
	public Integer getRecordId() {
        return recordId;
	}

    /**
     * Set the record id.
     * @param record
     */
	@SuppressWarnings("unused")
    private void setRecordId(Integer recordId) {
           this.recordId = recordId;
    }
	
	/**
	 * @return the latitude
	 */
	public float getLat() {
		return lat;
	}

	/**
	 * @param latitude the latitude to set
	 */
	public void setLat(float latitude) {
		this.lat = latitude;
	}

	/**
	 * @return the longitude
	 */
	public float getLon() {
		return lon;
	}

	/**
	 * @param longitude the longitude to set
	 */
	public void setLon(float longitude) {
		this.lon = longitude;
	}

	public Integer getIndex() {
		return index;
	}

	public void setIndex(Integer index) {
		this.index = index;
	}

}
