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
package com.raytheon.uf.common.dataplugin.shef.tables;

// Generated Oct 17, 2008 2:22:17 PM by Hibernate Tools 3.2.2.GA

import java.util.Date;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.EmbeddedId;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Transient;

import com.raytheon.uf.common.dataplugin.persist.PersistableDataObject;
import com.raytheon.uf.common.serialization.annotations.DynamicSerialize;
import com.raytheon.uf.common.serialization.annotations.DynamicSerializeElement;

/**
 * Dhrradar generated by hbm2java
 *
 *
 * <pre>
 *
 * SOFTWARE HISTORY
 * Date         Ticket#    Engineer    Description
 * ------------ ---------- ----------- --------------------------
 * Oct 17, 2008                        Initial generation by hbm2java
 * Aug 19, 2011 10672      jkorman     Move refactor to new project
 * Oct 07, 2013 2361       njensen     Removed XML annotations
 * Sep 26, 2016 5631       bkowal      Cleanup.
 * Jul 25, 2018 5588       mapeters    Added NamedQueries annotation
 *
 * </pre>
 *
 * @author jkorman
 */
@NamedQueries({
        @NamedQuery(name = Dhrradar.SELECT_BY_RAD_ID_BETWEEN_OBS_TIME, query = Dhrradar.SELECT_BY_RAD_ID_BETWEEN_OBS_TIME_HQL) })
@Entity
@Table(name = "dhrradar")
@DynamicSerialize
public class Dhrradar extends PersistableDataObject<DhrradarId>
        implements IGriddedRadarRecord {

    private static final long serialVersionUID = 1L;

    public static final String SELECT_BY_RAD_ID_BETWEEN_OBS_TIME = "selectDHRRadarByRadIdBetweenObsTime";

    protected static final String SELECT_BY_RAD_ID_BETWEEN_OBS_TIME_HQL = "FROM Dhrradar r WHERE r.id.radid = :radid AND r.id.obstime >= :startObsTime AND r.id.obstime <= :endObsTime";

    @DynamicSerializeElement
    private DhrradarId id;

    @DynamicSerializeElement
    private Radarloc radarloc;

    @DynamicSerializeElement
    private Short volcovpat;

    @DynamicSerializeElement
    private Short opermode;

    @DynamicSerializeElement
    private Float dbzmin;

    @DynamicSerializeElement
    private Float dbzinc;

    @DynamicSerializeElement
    private Float dbzcnt;

    @DynamicSerializeElement
    private Short JDate;

    @DynamicSerializeElement
    private Short JTime;

    @DynamicSerializeElement
    private Short meanFieldBias;

    @DynamicSerializeElement
    private Short sampleSize;

    @DynamicSerializeElement
    private String gridFilename;

    public Dhrradar() {
    }

    public Dhrradar(DhrradarId id, Radarloc radarloc) {
        this.id = id;
        this.radarloc = radarloc;
    }

    public Dhrradar(DhrradarId id, Radarloc radarloc, Short volcovpat,
            Short opermode, Float dbzmin, Float dbzinc, Float dbzcnt,
            Short JDate, Short JTime, Short meanFieldBias, Short sampleSize,
            String gridFilename) {
        this.id = id;
        this.radarloc = radarloc;
        this.volcovpat = volcovpat;
        this.opermode = opermode;
        this.dbzmin = dbzmin;
        this.dbzinc = dbzinc;
        this.dbzcnt = dbzcnt;
        this.JDate = JDate;
        this.JTime = JTime;
        this.meanFieldBias = meanFieldBias;
        this.sampleSize = sampleSize;
        this.gridFilename = gridFilename;
    }

    @Transient
    @Override
    public Date getObsTime() {
        return id.getObstime();
    }

    @EmbeddedId
    @AttributeOverrides({
            @AttributeOverride(name = "radid", column = @Column(name = "radid", nullable = false, length = 3)),
            @AttributeOverride(name = "obstime", column = @Column(name = "obstime", nullable = false, length = 29)) })
    public DhrradarId getId() {
        return this.id;
    }

    public void setId(DhrradarId id) {
        this.id = id;
    }

    @ManyToOne(fetch = FetchType.EAGER)
    @JoinColumn(name = "radid", nullable = false, insertable = false, updatable = false)
    public Radarloc getRadarloc() {
        return this.radarloc;
    }

    public void setRadarloc(Radarloc radarloc) {
        this.radarloc = radarloc;
    }

    @Column(name = "volcovpat")
    public Short getVolcovpat() {
        return this.volcovpat;
    }

    public void setVolcovpat(Short volcovpat) {
        this.volcovpat = volcovpat;
    }

    @Column(name = "opermode")
    public Short getOpermode() {
        return this.opermode;
    }

    public void setOpermode(Short opermode) {
        this.opermode = opermode;
    }

    @Column(name = "dbzmin", precision = 8, scale = 8)
    public Float getDbzmin() {
        return this.dbzmin;
    }

    public void setDbzmin(Float dbzmin) {
        this.dbzmin = dbzmin;
    }

    @Column(name = "dbzinc", precision = 8, scale = 8)
    public Float getDbzinc() {
        return this.dbzinc;
    }

    public void setDbzinc(Float dbzinc) {
        this.dbzinc = dbzinc;
    }

    @Column(name = "dbzcnt", precision = 8, scale = 8)
    public Float getDbzcnt() {
        return this.dbzcnt;
    }

    public void setDbzcnt(Float dbzcnt) {
        this.dbzcnt = dbzcnt;
    }

    @Column(name = "j_date")
    public Short getJDate() {
        return this.JDate;
    }

    public void setJDate(Short JDate) {
        this.JDate = JDate;
    }

    @Column(name = "j_time")
    public Short getJTime() {
        return this.JTime;
    }

    public void setJTime(Short JTime) {
        this.JTime = JTime;
    }

    @Override
    @Column(name = "mean_field_bias")
    public Short getMeanFieldBias() {
        return this.meanFieldBias;
    }

    public void setMeanFieldBias(Short meanFieldBias) {
        this.meanFieldBias = meanFieldBias;
    }

    @Column(name = "sample_size")
    public Short getSampleSize() {
        return this.sampleSize;
    }

    public void setSampleSize(Short sampleSize) {
        this.sampleSize = sampleSize;
    }

    @Override
    @Column(name = "grid_filename", length = 20)
    public String getGridFilename() {
        return this.gridFilename;
    }

    public void setGridFilename(String gridFilename) {
        this.gridFilename = gridFilename;
    }

}
