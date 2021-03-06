//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vJAXB 2.1.10 in JDK 6 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2012.01.10 at 10:31:03 AM CST 
//


package org.isotc211._2005.gfc;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.isotc211._2005.gco.AbstractObjectType;
import org.isotc211._2005.gco.BooleanPropertyType;
import org.isotc211._2005.gco.CharacterStringPropertyType;


/**
 * FC_InheritanceRelation realizes GF_InheritanceRelation.  - [ocl] - FC_InheritanceRelation always assumes that its GF_InheritanceRelation::uniqueInstance is TRUE. - [/ocl]
 * 
 * <p>Java class for FC_InheritanceRelation_Type complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="FC_InheritanceRelation_Type">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.isotc211.org/2005/gco}AbstractObject_Type">
 *       &lt;sequence>
 *         &lt;element name="name" type="{http://www.isotc211.org/2005/gco}CharacterString_PropertyType" minOccurs="0"/>
 *         &lt;element name="description" type="{http://www.isotc211.org/2005/gco}CharacterString_PropertyType"/>
 *         &lt;element name="uniqueInstance" type="{http://www.isotc211.org/2005/gco}Boolean_PropertyType"/>
 *         &lt;element name="subtype" type="{http://www.isotc211.org/2005/gfc}FC_FeatureType_PropertyType"/>
 *         &lt;element name="supertype" type="{http://www.isotc211.org/2005/gfc}FC_FeatureType_PropertyType"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "FC_InheritanceRelation_Type", propOrder = {
    "name",
    "description",
    "uniqueInstance",
    "subtype",
    "supertype"
})
public class FCInheritanceRelationType
    extends AbstractObjectType
{

    protected CharacterStringPropertyType name;
    @XmlElement(required = true)
    protected CharacterStringPropertyType description;
    @XmlElement(required = true)
    protected BooleanPropertyType uniqueInstance;
    @XmlElement(required = true)
    protected FCFeatureTypePropertyType subtype;
    @XmlElement(required = true)
    protected FCFeatureTypePropertyType supertype;

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link CharacterStringPropertyType }
     *     
     */
    public CharacterStringPropertyType getName() {
        return name;
    }

    /**
     * Sets the value of the name property.
     * 
     * @param value
     *     allowed object is
     *     {@link CharacterStringPropertyType }
     *     
     */
    public void setName(CharacterStringPropertyType value) {
        this.name = value;
    }

    /**
     * Gets the value of the description property.
     * 
     * @return
     *     possible object is
     *     {@link CharacterStringPropertyType }
     *     
     */
    public CharacterStringPropertyType getDescription() {
        return description;
    }

    /**
     * Sets the value of the description property.
     * 
     * @param value
     *     allowed object is
     *     {@link CharacterStringPropertyType }
     *     
     */
    public void setDescription(CharacterStringPropertyType value) {
        this.description = value;
    }

    /**
     * Gets the value of the uniqueInstance property.
     * 
     * @return
     *     possible object is
     *     {@link BooleanPropertyType }
     *     
     */
    public BooleanPropertyType getUniqueInstance() {
        return uniqueInstance;
    }

    /**
     * Sets the value of the uniqueInstance property.
     * 
     * @param value
     *     allowed object is
     *     {@link BooleanPropertyType }
     *     
     */
    public void setUniqueInstance(BooleanPropertyType value) {
        this.uniqueInstance = value;
    }

    /**
     * Gets the value of the subtype property.
     * 
     * @return
     *     possible object is
     *     {@link FCFeatureTypePropertyType }
     *     
     */
    public FCFeatureTypePropertyType getSubtype() {
        return subtype;
    }

    /**
     * Sets the value of the subtype property.
     * 
     * @param value
     *     allowed object is
     *     {@link FCFeatureTypePropertyType }
     *     
     */
    public void setSubtype(FCFeatureTypePropertyType value) {
        this.subtype = value;
    }

    /**
     * Gets the value of the supertype property.
     * 
     * @return
     *     possible object is
     *     {@link FCFeatureTypePropertyType }
     *     
     */
    public FCFeatureTypePropertyType getSupertype() {
        return supertype;
    }

    /**
     * Sets the value of the supertype property.
     * 
     * @param value
     *     allowed object is
     *     {@link FCFeatureTypePropertyType }
     *     
     */
    public void setSupertype(FCFeatureTypePropertyType value) {
        this.supertype = value;
    }

}
