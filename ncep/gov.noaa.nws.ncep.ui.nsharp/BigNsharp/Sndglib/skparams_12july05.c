/***************************************************************/
/*  SHARP-95                                                   */
/*  Advanced Interactive Sounding Analysis Program             */
/*                                                             */
/*  Thermodynamic Parameter Calculations                       */
/*                                                             */
/*  John A. Hart                                               */
/*  National Severe Storms Forecast Center                     */
/*  Kansas City, Missouri                                      */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sndglib.h"
#include "skparams.h"

/* Global for this file only */
float dcape_entrain=ENTRAIN_DEFAULT;

/* define_parcel() Modifies global var lplvals */

void define_parcel(short flag, float pres)
	/*************************************************************/
	/*  DEFINE_PARCEL                                            */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Chooses LPL, and assigns values to LPLVALS struct.       */
	/*                                                           */
	/*  flag   -   Parcel selection.                             */
	/*            -1 = Use Previous Selection                    */
	/*             1 = Observed sfc parcel                       */
	/*             2 = Fcst sfc parcel                           */
	/*             3 = Most unstable parcel                      */
	/*             4 = Mean mixlyr parcel                        */
	/*             5 = User defined parcel                       */
	/*             6 = Mean Effective Layer parcel               */
	/*                                                           */
	/*  pres   -   Variable pressure level (mb)                  */
	/*************************************************************/
{
	short idxt, idxtd, idxp, i;
	float mmr, mtha, ix1 = 0, p_top, p_bot;

	/* Initialize lplvals to no definition */
	lplvals.flag = (int)RMISSD;
	lplvals.temp = RMISSD;
	lplvals.dwpt = RMISSD;
	lplvals.pres = RMISSD;

	if (!sndg || numlvl < 1) return;

	idxt  = getParmIndex("TEMP");
	idxtd = getParmIndex("DWPT");
	idxp  = getParmIndex("PRES");

	if (idxt == -1 || idxtd == -1 || idxp == -1) return;

	if (flag == -1) flag = lplvals.flag;

	switch (flag) {
	  case 1:
	    strcpy(lplvals.desc, "SFC PARCEL");
	    i = sfc();
	    lplvals.temp = sndg[i][idxt];
	    lplvals.dwpt = sndg[i][idxtd];
	    lplvals.pres = sndg[i][idxp];
	  break;

	  case 2:
	    strcpy(lplvals.desc, "FCST SFC PARCEL");
	    lplvals.temp = max_temp(&ix1, -1);
	    mmr = mean_mixratio(&ix1, -1, -1);
	    lplvals.dwpt = temp_at_mixrat(mmr, sndg[sfc()][idxp]);
	    lplvals.pres = sndg[sfc()][idxp];
	  break;

	  case 3:
	    ix1 = unstbl_lvl(&ix1, -1, sndg[sfc()][idxp]-pres);
	    sprintf(lplvals.desc, "MU PARCEL IN LOWEST %dmb", (short)pres);
	    lplvals.pres = ix1;
	    lplvals.temp = i_temp(ix1, I_PRES);
	    lplvals.dwpt = i_dwpt(ix1, I_PRES);
	  break;

	  case 4:
	    sprintf(lplvals.desc, "%dmb MIXED LAYER PARCEL", (short)pres);
	    mtha = mean_theta(&ix1, -1, sndg[sfc()][idxp]-pres);
	    lplvals.temp = theta(1000.0, mtha, sndg[sfc()][idxp]);
	    mmr = mean_mixratio(&ix1, -1, sndg[sfc()][idxp]-pres);
	    lplvals.dwpt = temp_at_mixrat(mmr, sndg[sfc()][idxp]);
	    lplvals.pres = sndg[sfc()][idxp];
	  break;

	  case 5:
	    sprintf(lplvals.desc, "%d mb %s", (short)pres, "PARCEL");
	    lplvals.pres = pres;
	    lplvals.temp = i_temp(pres, I_PRES);
	    lplvals.dwpt = i_dwpt(pres, I_PRES);
	  break;

	  case 6:
/*	    printf( "Setting Effective Layer Parcel\n"); */
	    effective_inflow_layer(100,-250, &p_bot, &p_top);
	    if (p_bot > 0) {
	       sprintf(lplvals.desc, "%dmb MEAN EFFECTIVE LAYER PARCEL", (int)(p_bot - p_top));
	       mtha = mean_theta(&ix1, p_bot, p_top);
	       lplvals.temp = theta(1000.0, mtha, (p_top + p_bot) / 2);
	       mmr = mean_mixratio(&ix1, p_bot, p_top);
	       lplvals.dwpt = temp_at_mixrat(mmr, (p_top + p_bot) / 2);
	       lplvals.pres = (p_top + p_bot) / 2; }
	  else {
               i = sfc();
               lplvals.temp = sndg[i][idxt];
               lplvals.dwpt = sndg[i][idxtd];
               lplvals.pres = sndg[i][idxp]; }
	  break;
	}

	lplvals.flag    = flag;
	lplvals.presval = pres;
}


float k_index(float *param)
        /*************************************************************/
        /*  K_INDEX                                                  */
        /*  John Hart  NSSFC KCMO                                    */
        /*                                                           */
        /*  Calculates the K-Index from data in SNDG array.          */
        /*  Value is returned both as (param) and as a RETURN;.      */
        /*************************************************************/
{
        float t8, t5, t7, td7, td8;

        t8 = i_temp(850.0, I_PRES);
        t7 = i_temp(700.0, I_PRES);
        t5 = i_temp(500.0, I_PRES);
        td7 = i_dwpt(700.0, I_PRES);
        td8 = i_dwpt(850.0, I_PRES);

	if (!qc(t8) || !qc(td8) || !qc(t7) || !qc(td7) || !qc(t5))
	  *param = RMISSD;
	else
          *param = t8 - t5 + td8 - (t7 - td7);

        return *param;
}


float t_totals(float *param, float *ct, float *vt)
        /*************************************************************/
        /*  T_TOTALS                                                 */
        /*  John Hart  NSSFC KCMO                                    */
        /*                                                           */
        /*  Calculates the Total Totals index from data in           */
        /*  SNDG array.  Value is returned both as (param) and as    */
        /*  a RETURN;.  Cross Totals (ct) and Vertical Totals(vt)    */
        /*  are also returned.                                       */
        /*************************************************************/
{
        float t8, t5, td8;

        t8 = i_temp(850.0, I_PRES);
        t5 = i_temp(500.0, I_PRES);
        td8 = i_dwpt(850.0, I_PRES);

	if (!qc(t8) || !qc(t5) || !qc(td8)) {
	  *param = RMISSD;
	  *vt = RMISSD;
	  *ct = RMISSD;
	}
	else {
          *vt = t8 - t5;
          *ct = td8 - t5;
          *param = *vt + *ct;
	}

        return *param;
}


float precip_water(float *param, float lower, float upper)
        /*************************************************************/
        /*  PRECIP_WATER                                             */
        /*  John Hart  NSSFC KCMO                                    */
        /*                                                           */
        /*  Calculates the Precipitable Water from data in           */
        /*  SNDG array within specified layer.                       */
        /*  Value is returned both as (param) and as a RETURN;.      */
        /*                                                           */
        /*  *param      =  Returned precipitable water value (in)    */
        /*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
        /*  upper       =  Top level of layer (mb)    [ -1=400]      */
        /*************************************************************/
{
        short i, okl, oku, lptr, uptr, idxp, idxtd;
        float d1, p1, d2, p2, tot, w1, w2, wbar;

	*param = RMISSD;

	if (!sndg || numlvl < 1)
	  return RMISSD;

	if (lower == RMISSD) return RMISSD;

	idxp  = getParmIndex("PRES");
	idxtd = getParmIndex("DWPT");

	if (idxp == -1 || idxtd == -1)
	  return RMISSD;

        /* ----- See if default layer is specified ----- */
        if (lower == -1) 
	  lower = sndg[sfc()][idxp];
        if (upper == -1)
	  upper = 400.0;

        /* ----- Make sure this is a valid layer ----- */
        while (!qc(i_dwpt(upper, I_PRES)) && upper < 800.0)
	  upper += 50.0;
        if (!qc(i_temp(lower, I_PRES)))
	  lower = i_pres(sfc());

	/* Make sure our sounding isn't too shallow 
        if (upper > 700.0) {
           *param = RMISSD;
           return *param;
        }
	*/

        /* ----- Find lowest observation in layer ----- */
        i = 0;
        while (sndg[i][idxp] > lower)
	  i++;
        while (!qc(sndg[i][idxtd]))
	  i++;
        lptr = i;
        if (sndg[i][idxp] == lower)
	  lptr++;

        /* ----- Find highest observation in layer ----- */
        i=numlvl-1;
        while (sndg[i][idxp] < upper) 
	  i--;
        uptr = i;
          if (sndg[i][idxp] == upper)
	uptr--;

        /* ----- Start with interpolated bottom layer ----- */
        d1 = i_dwpt(lower, I_PRES);
        p1 = lower;

        tot = 0;
        for (i = lptr; i <= uptr; i++) {
          if (qc(sndg[i][idxtd])) {
            /* ----- Calculate every level that reports a dwpt ----- */
            d2 = sndg[i][idxtd];
            p2 = sndg[i][idxp];
            w1 = mixratio(p1, d1);
            w2 = mixratio(p2, d2);
            wbar = (w1 + w2) / 2;
            tot = tot + wbar * (p1 - p2);
            d1 = d2;
            p1 = p2;
          }
        }

        /* ----- Finish with interpolated top layer ----- */
        d2 = i_dwpt(upper, I_PRES);
        p2 = upper;
        w1 = mixratio(p1, d1);
        w2 = mixratio(p2, d2);
        wbar = (w1 + w2) / 2.0;
        tot = tot + wbar * (p1 - p2);

        /* ----- Convert to inches (from g*mb/kg) ----- */
        *param = tot * 0.00040173;

	return *param;
}

/* Mike hack */
float parcelx(float lower, float upper, float pres, float temp,
	     float dwpt, Parcel *pcl)
{
	return parcel(lower, upper, pres, temp, dwpt, pcl);
}

float parcel(float lower, float upper, float pres, float temp,
	     float dwpt, Parcel *pcl)
	/*************************************************************/
	/*  PARCEL                                                   */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Lifts specified parcel, calculating various levels and   */
	/*  parameters from data in SNDG array.  B+/B- are           */
	/*  calculated based on layer (lower-upper).  Bplus is       */
	/*  returned value and in structure.                         */
	/*                                                           */
	/*  All calculations use the virtual temperature correction. */
	/*                                                           */
	/*  lower       =  Bottom level of layer (mb)  [ -1=SFC]     */
	/*  upper       =  Top level of layer (mb)     [ -1=TOP]     */
	/*  pres        =  LPL pressure (mb)                         */
	/*  temp        =  LPL temperature (c)                       */
	/*  dwpt        =  LPL dew point (c)                         */
	/*  pcl         =  returned _parcel structure                */
	/*************************************************************/
{
	short i, idxp, idxz, idxt, idxtd;
	short lptr, uptr;
	float te1, pe1, te2, pe2, h1, h2, lyre, tdef1, tdef2, totp, totn;
	float te3, pe3, h3, tp1, tp2, tp3, tdef3, lyrf, lyrlast, pelast;
	float tote, totx, ls1, cap_strength, li_max, li_maxpres, blupper;
	float cap_strengthpres, tpx, ix1, hgt10c, pp, pp1,pp2, dz,dpt,det;
	float bltheta, blmr;
	float theta_parcel, tv_env_bot, tv_env_top;

	/* ----- Initialize PCL values ----- */
	pcl->lplpres     = pres;
	pcl->lpltemp     = temp;
	pcl->lpldwpt     = dwpt;
	pcl->blayer      = lower;
	pcl->tlayer      = upper;
	pcl->entrain     = 0;
	pcl->lclpres     = RMISSD;
	pcl->lfcpres     = RMISSD;
	pcl->elpres      = RMISSD;
	pcl->mplpres     = RMISSD;
	pcl->bplus       = RMISSD;
	pcl->bminus      = RMISSD;
	pcl->bfzl        = RMISSD;
	pcl->cape3km     = RMISSD;
	pcl->wm10c       = RMISSD;
	pcl->li5         = RMISSD;
	pcl->li3         = RMISSD;
	pcl->brn         = RMISSD;
	pcl->limax       = RMISSD;
	pcl->limaxpres   = RMISSD;
	pcl->cap         = RMISSD;
	pcl->cappres     = RMISSD;

	lyre             = -1;
	cap_strength     = RMISSD;
	cap_strengthpres = RMISSD;
	li_max           = RMISSD;
	li_maxpres       = RMISSD;
	totp             = 0;
	totn             = 0;

	/* Don't work on NULL pointers */
	if (!sndg || numlvl < 1)
	  return RMISSD;

	idxp  = getParmIndex("PRES");
	idxz  = getParmIndex("HGHT");
	idxt  = getParmIndex("TEMP");
	idxtd = getParmIndex("DWPT");

	if (idxp == -1 || idxt == -1 || idxtd == -1 || idxz == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (lower == -1) {
	   lower = sndg[sfc()][idxp];
	   pcl->blayer = lower;
	}
	if (upper == -1) {
	   upper = sndg[numlvl-1][idxp];
	   pcl->tlayer = upper;
	}

	/* ----- Make sure this is a valid layer ----- */
	if (lower > pres) {
	   lower = pres;
	   pcl->blayer = lower;
	}

	if (!qc(i_vtmp(upper, I_PRES)) ||
	    !qc(i_vtmp(lower, I_PRES))) {
	  return RMISSD;
	}

	/* ----- Begin with Mixing Layer (LPL-LCL) ----- */
	te1 = i_vtmp(pres, I_PRES);
	pe1 = lower;
	h1 =  i_hght(pe1, I_PRES);
	tp1 = virtemp(pres, temp, dwpt);

	/* lift parcel and return LCL pres (mb) and LCL temp (C) */
	drylift(pres, temp, dwpt, &pe2, &tp2);
	/* define top of layer as LCL pres */
	blupper = pe2;
	h2 =  i_hght(pe2, I_PRES);
	te2 = i_vtmp(pe2, I_PRES);
	pcl->lclpres = pe2;
	
	/* calculate lifted parcel theta for use in iterative CINH loop below */
	/* recall that lifted parcel theta is CONSTANT from LPL to LCL */
	theta_parcel = theta(pe2, tp2, 1000.0);
	
	/* environmental theta and mixing ratio at LPL */
	bltheta = theta(pres, i_temp(pres, I_PRES), 1000.0);
	blmr = mixratio(pres, dwpt);

	/* ----- Accumulate CINH in mixing layer below the LCL ----- */
	/* This will be done in 10mb increments, and will use the    */
	/* virtual temperature correction where possible.            */

	/* initialize negative area to zero and iterate from LPL to LCL in 10mb increments */
	totn=0;
	for (pp=lower; pp > blupper; pp-=10.0) {
	   pp1 = pp; 
	   pp2 = pp-10.0;
	   if (pp2 < blupper)
	     pp2=blupper;

	   dz = i_hght(pp2, I_PRES) - i_hght(pp1, I_PRES);

/*	   
	   tp1 = theta(1000.0, bltheta, pp1);
	   tp2 = theta(1000.0, bltheta, pp2);		
*/
	   /* calculate difference between Tv_parcel and Tv_environment at top and bottom of 10mb layers */	
	   /* make use of constant lifted parcel theta and mixr from LPL to LCL */
	   tv_env_bot = virtemp(pp1, theta(pp1, i_temp(pp1, I_PRES), 1000.0), i_dwpt(pp1, I_PRES));
           tdef1 = ((virtemp(pp1, theta_parcel, temp_at_mixrat(blmr, pp1))) - tv_env_bot) /
	           (tv_env_bot + 273.13);

/*	   tdef2 = (virtemp(pp2, theta_parcel, temp_at_mixrat(blmr, pp2)) - 
	     i_vtmp(pp2, I_PRES)) / (i_vtmp(pp2, I_PRES) + 273.15);
*/
	   tv_env_top = virtemp(pp2, theta(pp2, i_temp(pp2, I_PRES), 1000.0), i_dwpt(pp2, I_PRES));
           tdef2 = ((virtemp(pp2, theta_parcel, temp_at_mixrat(blmr, pp2))) - tv_env_top) /
                   (tv_env_top + 273.15);

           lyre = 9.8 * (tdef1 + tdef2) / 2.0 * dz;
	   if (lyre < 0)
	     totn+=lyre;
	}

	if (lower > pe2) {
	   lower = pe2;
	   pcl->blayer = lower;
	}

	te1 = i_vtmp(pres, I_PRES);
	pe1 = lower;
	h1 =  i_hght(pe1, I_PRES);
	tp1 = virtemp(pres, temp, dwpt);

	drylift(pres, temp, dwpt, &pe2, &tp2);
	h2 =  i_hght(pe2, I_PRES);
	te2 = i_vtmp(pe2, I_PRES);

	/* ----- Calculate height of -10c level ----- */
	hgt10c = i_hght(temp_lvl( -10.0, &ix1), I_PRES);

	/* ----- Find lowest observation in layer ----- */
	i = 0;
	while (sndg[i][idxp] > lower) {
	  if (i == numlvl-1) {
	    break;
	  }
	  i++;
	}
	while (!qc(sndg[i][idxtd])) {
	  if (i == numlvl-1) {
	    break;
	  }
	  i++;
	}
	lptr = i;
	if (sndg[i][idxp] == lower) {
	  if (i != numlvl-1) {
	    lptr++;
	  }
	}

	/* ----- Find highest observation in layer ----- */
	i=numlvl-1;
	while (sndg[i][idxp] < upper) {
	  if (i == 0) {
	    break;
	  }
	  i--;
	}
	uptr = i;
	if (sndg[i][idxp] == upper) {
	  if (i > 0) {
	    uptr--;
	  }
	}

	/* ----- Start with interpolated bottom layer ----- */
	/* begin moist ascent from lifted parcel LCL (pe2, tp2) */
	pe1 = lower;
	h1 =  i_hght(pe1, I_PRES);
	te1 = i_vtmp(pe1, I_PRES);
	tp1 = wetlift(pe2, tp2, pe1);

	totp = 0;
	lyre = 0;
	for (i = lptr; i < numlvl; i++) {
	  if (qc(sndg[i][idxt])) {
	    /* ----- Calculate every level that reports a temp ----- */		
	    pe2 = sndg[i][idxp];
	    h2 =  sndg[i][idxz];
	    te2 = i_vtmp(pe2, I_PRES);
	    tp2 = wetlift(pe1, tp1, pe2);
	    tdef1 = (virtemp(pe1, tp1, tp1) - te1) / (te1 + 273.15);
	    tdef2 = (virtemp(pe2, tp2, tp2) - te2) / (te2 + 273.15);
	    lyrlast = lyre;
	    lyre = 9.8 * (tdef1 + tdef2) / 2.0 * (h2 - h1);

	    /* ----- Check for Max LI ----- */
	    if ((virtemp(pe2, tp2, tp2) - te2) > li_max) {
	      li_max = virtemp(pe2, tp2, tp2) - te2;
	      li_maxpres = pe2;
	    }

	    /* ----- Check for Max Cap Strength ----- */
	    if ((te2 - virtemp(pe2, tp2, tp2)) > cap_strength) {
	      cap_strength = te2 - virtemp(pe2, tp2, tp2);
	      cap_strengthpres = pe2;
	    }

	    /* add layer energy to total positive if lyre > 0 */	
	    if (lyre > 0)
	      totp += lyre;
	    /* add layer energy to total negative if lyre < 0, only up to 500 mb */
	    else
	      if (pe2 > 500.0)
	        totn += lyre;

	    tote += lyre;
	    pelast = pe1;
	    pe1 = pe2;
	    h1 = h2;
	    te1 = te2;
	    tp1 = tp2;

	    /* ----- Is this the top of given layer ----- */
	    if ((i >= uptr) && (!qc(pcl->bplus))) {
	      pe3 = pe1;
	      h3 = h1;
	      te3 = te1;
	      tp3 = tp1;
	      lyrf = lyre;

	      if (lyrf > 0.0) {
		pcl->bplus = totp - lyrf;
		pcl->bminus = totn;
	      }
	      else {
		pcl->bplus = totp;
		if (pe2 > 500.0)
		  pcl->bminus = totn + lyrf;
		else
		  pcl->bminus = totn;
	      }

	      pe2 = upper;
	      h2 = i_hght(pe2, I_PRES);
	      te2 = i_vtmp(pe2, I_PRES);
	      tp2 = wetlift(pe3, tp3, pe2);
	      tdef3 = (virtemp(pe3, tp3, tp3) - te3) / (te3 + 273.15);
	      tdef2 = (virtemp(pe2, tp2, tp2) - te2) / (te2 + 273.15);
	      lyrf = 9.8 * (tdef3 + tdef2) / 2.0 * (h2 - h3);
	      if (lyrf > 0.0)
		pcl->bplus += lyrf;
	      else
		if (pe2 > 600.0)
	          pcl->bminus += lyrf;

	      if (pcl->bplus == 0.0)
	        pcl->bminus = 0.0;
	    }

	    /* ----- Is this the freezing level ----- */
	    if ((te2 <= 0.0) && (!qc(pcl->bfzl))) {
	      pe3 = pelast;
	      h3 = i_hght(pe3, I_PRES);
	      te3 = i_vtmp(pe3, I_PRES);
	      tp3 = wetlift(pe1, tp1, pe3);
	      lyrf = lyre;

	      if (lyrf > 0.0)
	        pcl->bfzl = totp - lyrf;
	      else
	        pcl->bfzl = totp;

	      if (qc(pe2 = temp_lvl(0, &pe2))) {
		h2 = i_hght(pe2, I_PRES);
		te2 = i_vtmp(pe2, I_PRES);
		tp2 = wetlift(pe3, tp3, pe2);
		tdef3 = (virtemp(pe3, tp3, tp3) - te3) / 
				(te3 + 273.15);
		tdef2 = (virtemp(pe2, tp2, tp2) - te2) / 
				(te2 + 273.15);
		lyrf = 9.8 * (tdef3 + tdef2) / 2.0 * (h2 - h3);
		if (lyrf > 0.0)
		  pcl->bfzl += lyrf;
	      }
	    }

            /* ----- Is this the -10c level ----- */
	    if ((te2 <= -10.0) && (!qc(pcl->wm10c))) {
	      pe3 = pelast;
	      h3 = i_hght(pe3, I_PRES);
	      te3 = i_vtmp(pe3, I_PRES);
	      tp3 = wetlift(pe1, tp1, pe3);
	      lyrf = lyre;

	      if (lyrf > 0.0)
	        pcl->wm10c = totp - lyrf;
	      else
	        pcl->wm10c = totp;

	      if (qc(pe2 = temp_lvl( -10.0, &pe2))) {
		h2 = i_hght(pe2, I_PRES);
		te2 = i_vtmp(pe2, I_PRES);
		tp2 = wetlift(pe3, tp3, pe2);
		tdef3 = (virtemp(pe3, tp3, tp3) - te3) / 
				(te3 + 273.15);
		tdef2 = (virtemp(pe2, tp2, tp2) - te2) / 
				(te2 + 273.15);
		lyrf = 9.8 * (tdef3 + tdef2) / 2.0 * (h2 - h3);
		if (lyrf > 0.0)
		  pcl->wm10c += lyrf;
	      }
	    }

	    /* ----- Is this the 3km level ----- */
	    if ((agl(i_hght(pe2, I_PRES)) >= 3000.0) && (!qc(pcl->cape3km))) {
	      pe3 = pelast;
	      h3 = i_hght(pe3, I_PRES);
	      te3 = i_vtmp(pe3, I_PRES);
	      tp3 = wetlift(pe1, tp1, pe3);
	      lyrf = lyre;

	      if (lyrf > 0.0)
	        pcl->cape3km = totp - lyrf;
	      else
	        pcl->cape3km = totp;

	      pe2 = i_pres(msl(3000));
	      if (qc(pe2)) {
		h2 = msl(3000.0);
		te2 = i_vtmp(pe2, I_PRES);
		tp2 = wetlift(pe3, tp3, pe2);
		tdef3 = (virtemp(pe3, tp3, tp3) - te3) / (te3 + 273.15);
		tdef2 = (virtemp(pe2, tp2, tp2) - te2) / (te2 + 273.15);
		lyrf = 9.8 * (tdef3 + tdef2) / 2.0 * (h2 - h3);
		if (lyrf > 0.0)
		  pcl->cape3km += lyrf;
	      }
	    }

	    /* ----- LFC Possibility ----- */
	    if ((lyre >= 0) && (lyrlast <= 0)) 
	      {
	      tp3 = tp1;
	      te3 = te1;
	      pe2 = pe1;
	      pe3 = pelast;
	      while (i_vtmp(pe3, I_PRES) > virtemp(pe3, wetlift(pe2, tp3, pe3), wetlift(pe2, tp3, pe3)))
		{ 
		pe3 -= 5; 
		}

	      pcl->lfcpres = pe3;

	      tote = 0;
	      pcl->elpres = RMISSD;

	      li_max = RMISSD;

	      if (cap_strength < 0.0) cap_strength = 0.0;
	      pcl->cap = cap_strength;
	      pcl->cappres = cap_strengthpres;
	    }

	    /* ----- EL Possibility ----- */
	    if ((lyre <= 0.0) && (lyrlast >= 0.0)) {
	      tp3 = tp1;
	      te3 = te1;
	      pe2 = pe1;
	      pe3 = pelast;
	      while (i_vtmp(pe3, I_PRES) < 
	        virtemp(pe3, wetlift(pe2, tp3, pe3), wetlift(pe2, tp3, pe3)))
	        pe3 -= 5.0;

	      pcl->elpres = pe3;
	      pcl->mplpres = RMISSD;
	      pcl->limax = -li_max;
	      pcl->limaxpres = li_maxpres;
	    }

	    /* ----- MPL Possibility ----- */
	    if (((tote <= 0.0) && (!qc(pcl->mplpres)) && (qc(pcl->elpres)))) {
	      pe3 = pelast;
	      h3 = i_hght(pe3, I_PRES);
	      te3 = i_vtmp(pe3, I_PRES);
	      tp3 = wetlift(pe1, tp1, pe3);
	      totx = tote - lyre;

	      pe2 = pelast;
	      while (totx > 0.0) {
		pe2 -= 1;
		te2 = i_vtmp(pe2, I_PRES);
		tp2 = wetlift(pe3, tp3, pe2);
		h2 = i_hght(pe2, I_PRES);
		tdef3 = (virtemp(pe3, tp3, tp3) - te3) / (te3 + 273.15);
		tdef2 = (virtemp(pe2, tp2, tp2) - te2) / (te2 + 273.15);
		lyrf = 9.8 * (tdef3 + tdef2) / 2.0 * (h2 - h3);
		totx += lyrf;

		tp3 = tp2;
		te3 = te2;
		pe3 = pe2;
	      }
	      pcl->mplpres = pe2;
	    }

              /* ----- 500mb Lifted Index ----- */
              if(( sndg[i][idxp] <= 500 ) && (pcl->li5 == RMISSD))
                 { pcl->li5 = i_vtmp(500, I_PRES) - virtemp(500, wetlift(pe1, tp1, 500), wetlift(pe1, tp1, 500)); }

              /* ----- 300mb Lifted Index ----- */
              if(( sndg[i][idxp] <= 300 ) && (pcl->li3 == RMISSD))
                 { pcl->li3 = i_vtmp(300, I_PRES) - virtemp(300, wetlift(pe1, tp1, 300), wetlift(pe1, tp1, 300)); }

	   }
	}    /* for() */

	/* ----- Calculate BRN if available ----- */
	pcl->brn = bulk_rich(*pcl, &ix1);

	return pcl->bplus;
}


float temp_lvl(float temp, float *param)
	/*************************************************************/
	/*  TEMP_LVL                                                 */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the level (mb) of the first occurrence of     */
	/*  (temp) in the environment.                               */
	/*                                                           */
	/*  *param      =  Returned level (mb).                      */
	/*  temp        =  Temperature (c) to search for.            */
	/*************************************************************/
{
	short  i, idxt, idxp;
	float  p0;
	double nm1, nm2, nm3;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxt = getParmIndex("TEMP");
	idxp = getParmIndex("PRES");
	if (idxt == -1 || idxp == -1)
	  return RMISSD;

	for (i=0; i < numlvl; i++) {
	  if ((qc(sndg[i][idxt])) && (sndg[i][idxt] <= temp)) {
	    if (i == 0)
	      return RMISSD;
	    if (sndg[i][idxt] == temp)
	      return sndg[i][idxp];

	    p0 = sndg[i-1][idxp];
	    nm1 = temp - i_temp(p0, I_PRES);
	    nm2 = sndg[i][idxt] - i_temp(p0, I_PRES);
	    nm3 = log(sndg[i][idxp] / p0);
	    *param = (float)(p0 * exp((nm1 / nm2) * nm3));
	    return *param;
	  }
	}
	return RMISSD;
}


float wb_lvl(float temp, float *param)
	/*************************************************************/
	/*  WB_LVL                                                   */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the level (mb) of the given Wet-bulb          */
	/*  temperature.                                             */
	/*                                                           */
	/*  *param      =  Returned level (mb).                      */
	/*  temp        =  Wet-bulb temperature (c) to search for.   */
	/*************************************************************/
{
	short  i, idxt, idxtd, idxp;
	float  p0;
	double nm1, nm2, nm3, wb1, wb0;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxt  = getParmIndex("TEMP");
	idxtd = getParmIndex("DWPT");
	idxp  = getParmIndex("PRES");

	if (idxp == -1 || idxt == -1 || idxtd == -1)
	  return RMISSD;

	for (i=0; i < numlvl; i++) {
	  if ((qc(sndg[i][idxt])) && (qc(sndg[i][idxtd]))) {
	    wb1 = wetbulb(sndg[i][idxp], sndg[i][idxt], sndg[i][idxtd]);
	    if (wb1 < temp) {
	      if (i == 0)
	        return RMISSD;
	      if (wb1 == temp)
	        return sndg[i][idxp];

	      p0 = sndg[i-1][idxp];
	      wb0 = wetbulb(p0, i_temp(p0, I_PRES), i_dwpt(p0, I_PRES));
	      nm1 = temp - wb0;
	      nm2 = wb1 - wb0;
	      nm3 = log(sndg[i][idxp] / p0);

	      *param = (float)(p0 * exp(( nm1 / nm2) * nm3));
	      return *param;
	    }
	  }
	}
	return RMISSD;
}


float top_moistlyr(float *param)
	/*************************************************************/
	/*  TOP_MOISTLYR                                             */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Determines the top of the moist layer of a given         */
	/*  environment.  Criteria:  50% dcrs in q in 50mb.          */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned top of moist layer (mb)          */
	/*************************************************************/
{
	short i, idxp, idxt, idxtd;
	float num, p1, p2, q1, q2, mq1, mq2, mh1, mh2, dqdz, dz;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp  = getParmIndex("PRES");
	idxt  = getParmIndex("TEMP");
	idxtd = getParmIndex("DWPT");

	q1  = mixratio(sndg[sfc()][idxp], sndg[sfc()][idxtd]);
	p1  = sndg[sfc()][idxp];
	mq1 = q1;
	mh1 = i_hght(p1, I_PRES);
	for (i = sfc() + 1; i < numlvl; i++) {
	  if (qc(sndg[i][idxtd]) && qc(sndg[i][idxt])) {
	    /* ----- Calculate every level that reports a dwpt ----- */
	    p2 = sndg[i][idxp];
	    q2 = mixratio(p2, sndg[i][idxtd]);

	    mq2 = (q1 + q2) / 2.0;
	    mh2 = (i_hght(p2, I_PRES) + i_hght(p1, I_PRES)) / 2.0;
	    dz = mh2 - mh1;
	    if (dz == 0.0)
	      dz = 0.001;
	    dqdz = (mq2 - mq1) / dz;
	    if ((dqdz < -0.01) && (i > sfc()+1) && (sndg[i][idxp] >= 500.0)) {
	      *param = p1;
	      return *param;
	    }
	    q1  = q2;
	    p1  = p2;
	    mq1 = mq2;
	    mh1 = mh2;
	    num += 1;
	  }
	  *param = RMISSD;
	}

	return *param;
}


float max_temp(float *param, float mixlyr)
	/*************************************************************/
	/*  MAX_TEMP                                                 */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates a maximum temperature forecast based on       */
	/*  depth of mixing level and low level temperatures.        */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned max temp (c)                     */
	/*  mixlyr      =  Assumed depth of mixing layer [-1=100mb]  */
	/*************************************************************/
{
	short idxp;
	float t1, temp, sfcpres;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp = getParmIndex("PRES");
	if (idxp == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (mixlyr == -1) 
	  mixlyr = sndg[sfc()][idxp] - 100.0;

	temp = i_temp(mixlyr, I_PRES) + 273.15 + 2.0;
	sfcpres = sndg[sfc()][idxp];
	*param = (temp * pow( sfcpres / mixlyr , ROCP)) - 273.15;

	return *param;
}


float delta_t(float *param)
	/*************************************************************/
	/*  DELTA-T                                                  */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the delta-t index from data in SNDG array.    */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*************************************************************/
{
	if (!qc(i_temp(700.0, I_PRES)) || !qc( i_temp(500.0, I_PRES)))
	  *param = RMISSD;
	else
	   *param = i_temp(700.0, I_PRES) - i_temp(500.0, I_PRES);
	return *param;
}


float vert_tot(float *param)
	/*************************************************************/
	/*  VERT_TOT                                                 */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the vertical totals from data in SNDG array.  */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*************************************************************/
{
	if (!qc(i_vtmp(850.0, I_PRES)) || !qc(i_vtmp(500.0, I_PRES)))
	  *param = RMISSD;
	else
	  *param = i_vtmp(850.0, I_PRES) - i_vtmp(500.0, I_PRES);
	return *param;
}


float lapse_rate(float *param, float lower, float upper)
	/*************************************************************/
	/*  LAPSE_RATE                                               */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the lapse rate (C/km) from SNDG array.        */
	/*  Value is returned both as (param) and as a RETURN.       */
	/*************************************************************/
{
	float dt, dz, lr;

	if (!qc(i_vtmp(lower, I_PRES)) || !qc(i_vtmp(upper, I_PRES)))
	   *param = RMISSD;
	else {
	   dt = i_vtmp(upper , I_PRES) - i_vtmp(lower , I_PRES);
	   dz = i_hght(upper , I_PRES) - i_hght(lower , I_PRES);
	   *param = (dt / dz) * -1000.0;
	}
	return *param;
}


float bulk_rich(Parcel pcl, float *brnshear)
	/*************************************************************/
	/*  BRN                                                      */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Bulk Richardson Number for given parcel.  */
	/*  Value is returned from function. BRN shear as returned   */
        /*  in argument.                                             */
	/*************************************************************/
{
	short idxp;
	float ptop, pbot, x1, y1, x2, y2, z1, z2, dx, dy;

	*brnshear = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp = getParmIndex("PRES");
	if (idxp == -1)
	  return RMISSD;

	/* Make sure they've initialized a parcel here */
	if (lplvals.flag == (int)RMISSD) {
	  return RMISSD;
	}
	else if (lplvals.flag > 0 && lplvals.flag < 4) {
   	   ptop = i_pres(msl(6000.0));
	   pbot = sndg[sfc()][idxp];
	}
	else {
	   pbot = i_pres(i_hght(pcl.lplpres, I_PRES) - 500.0);
	   if (!qc(pbot)) 
	     pbot = sndg[sfc()][idxp];
	   ptop = i_pres(i_hght(pbot, I_PRES) + 6000.0);
	}

	/* ----- First, calculate lowest 500m mean wind ----- */
	mean_wind(pbot, i_pres(i_hght(pbot, I_PRES)+500.0), &x1, &y1, &z1, &z2);

	/* ----- Next, calculate 6000m mean wind ----- */
	mean_wind(pbot, ptop, &x2, &y2, &z1, &z2);

	/* ----- Check to make sure CAPE and SHEAR are avbl ----- */
	if (!qc(pcl.bplus) || !qc(x1) || !qc(x2))
	  return RMISSD;

	/* ----- Calculate shear between winds ----- */
	dx = x2 - x1;
	dy = y2 - y1;
	*brnshear = (float)(sqrt((dx * dx) + (dy * dy))) * 0.51479;
	*brnshear = *brnshear * *brnshear / 2.0;

	/* ----- Calculate and return BRN ----- */
	return pcl.bplus / *brnshear;
}


float cnvtv_temp(float *param, float mincinh)
	/*************************************************************/
	/*  CNVTV_TEMP                                               */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Computes convective temperature, assuming no change in   */
	/*  moisture profile.  Parcels are iteratively lifted until  */
	/*  only (mincinh) j/kg are left as a cap.  The first guess  */
	/*  is the observed surface temperature.                     */
	/*************************************************************/
/*
-mkay added default -50.0 for mincinh if mincinh == -1
*/
{
	short idxp, idxt;
	float  mmr, ix1, pres, te, tm, sfcpres, sfctemp, sfcdwpt, hct, hcd;
	Parcel pcl;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp = getParmIndex("PRES");
	idxt = getParmIndex("TEMP");

	if (idxp == -1 || idxt == -1)
	  return RMISSD;

	if ((int)mincinh == -1)
	  mincinh = -50.0;

	mmr = mean_mixratio(&ix1, -1, -1);
	sfcpres = sndg[sfc()][idxp];
	sfctemp = sndg[sfc()][idxt];
	sfcdwpt = temp_at_mixrat( mmr, sfcpres);

	ix1 = parcel(-1, -1, sfcpres, sfctemp, sfcdwpt, &pcl);

	/* 
	 * Do a quick search to find whether to continue.  
	 * If you need to heat up more that 15C, don't compute.
	 */
	ix1 = parcel(-1, -1, sfcpres, sfctemp+15, sfcdwpt, &pcl);
	if (pcl.bminus < mincinh) {
	   *param = RMISSD;
	   return *param;
	}
	
	ix1 = parcel( -1, -1, sfcpres, sfctemp, sfcdwpt, &pcl);
	while ((pcl.bminus < mincinh ) && (ix1 != RMISSD)) {
	  if((mincinh - pcl.bminus) > 100.0) {
	     sfctemp += 2.0;
	  }
	  else {
	     sfctemp += 1.0;
	  }
	  ix1 = parcel(-1, -1, sfcpres, sfctemp, sfcdwpt, &pcl);
	}

	if (ix1 == RMISSD)
	  *param = RMISSD;
	else
	  *param = sfctemp;
	return *param;
}


float sweat_index(float *param)
	/*************************************************************/
	/*  SWEAT_INDEX                                              */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the SWEAT-Index from data in SNDG array.      */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*************************************************************/
{
	float ix1, ix2, ix3, d8, tt, wsp8, wsp5, sw, wd8, wd5, sinw, angl;

	d8 = i_dwpt(850.0, I_PRES);
	tt = t_totals(&ix1, &ix2, &ix3);
	wsp8 = i_wspd(850.0, I_PRES);
	wsp5 = i_wspd(500.0, I_PRES);
	wd8 = i_wdir(850.0, I_PRES);
	wd5 = i_wdir(500.0, I_PRES);

	if (!qc(d8) || !qc(tt) || !qc(wsp8) || !qc(wsp5) || !qc(wd8) 
	    || !qc(wd5)) {
	  *param = RMISSD;
	  return *param;
	}

	sinw = 0;

	if (d8 > 0.0)
	  sw = 12.0 * d8;
	if (tt > 49.0) 
	  sw = sw + (20.0 * (tt - 49.0));
	sw = sw + (2.0 * wsp8) + wsp5;

	if ((wd8 >= 130.0) && (wd8 <= 250.0)) {
	  if ((wd5 >= 210.0) && (wd5 <= 310.0)) {
	    if (wd5 > wd8) {
	      angl = (wd5 - wd8) * (PI / 180.0);
	      sinw = (float)sin(angl);
	    }
	  }
	}
	if (sinw > 0.0)
	  sw = sw + (125.0 * (sinw + 0.2));
	*param = sw;

	return *param;
}


float old_cnvtv_temp(float *param)
	/*************************************************************/
	/*  OLD_CNVTV_TEMP                                           */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Computes convective temperature, assuming no change in   */
	/*  moisture profile.  Intersection of mixing ratio line and */
	/*  temperature profile is found, then the dry adiabat is    */
	/*  followed back to the surface pressure.                   */
	/*************************************************************/
{
	short idxp;
	float mmr, ix1, pres, te, tm;

	*param = RMISSD;

	if (!sndg || numlvl < 1)
	  return RMISSD;

	idxp = getParmIndex("PRES");
	if (idxp == -1)
	  return RMISSD;

	/* ----- Compute Historical Convective Temperature ----- */
	mmr = mean_mixratio(&ix1, -1, -1);
	for (pres = sndg[sfc()][idxp]; pres > sndg[numlvl-1][idxp]; pres -= 5.0)
	{
	   te = i_temp(pres, I_PRES);
	   tm = temp_at_mixrat(mmr, pres);
	   if(tm >= te) {
	      te += 273.15;
	      *param = (te * pow( sndg[sfc()][idxp] / pres , ROCP)) - 273.15;
	      return *param;
	   }
	}
	*param = RMISSD;
	return *param;
}


float unstbl_lvl(float *param, float lower, float upper)
	/*************************************************************/
	/*  UNSTBL_LVL                                               */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Finds the most unstable level between levels upper and   */
	/*  lower.                                                   */
	/*                                                           */
	/*  *param      =  Returned most unstable level (mb)         */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=low 300mb]*/
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr, idxp, idxt, idxtd;
	float t1, p2, t2, tmax, pmax, pres, temp, dwpt;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp  = getParmIndex("PRES");
	idxt  = getParmIndex("TEMP");
	idxtd = getParmIndex("DWPT");
	if (idxp == -1 || idxt == -1 || idxtd == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (lower == -1) { lower = sndg[sfc()][idxp];    }
	if (upper == -1) { upper = sndg[sfc()][idxp] - 300.0; }

	/* ----- Make sure this is a valid layer ----- */
	while (!qc(i_dwpt(upper, I_PRES))) { upper += 50.0; }
	if (!qc(i_temp(lower, I_PRES))) { lower = i_pres(sfc()); }

	/* Find lowest observation in layer ----- */
	i = 0;
	while(sndg[i][idxp] > lower)  { i++; }
	while (!qc(sndg[i][idxt])) { i++; }
	lptr = i;
	if( sndg[i][idxp] == lower ) { lptr++; }

	/* ----- Find highest observation in layer ----- */
	i=numlvl-1;
	while( sndg[i][idxp] < upper) { i--;}
	uptr = i;
	if( sndg[i][idxp] == upper ) { uptr--; }

	/* ----- Start with interpolated bottom layer ----- */
	drylift( lower, i_temp(lower, I_PRES), i_dwpt(lower, I_PRES), &p2, &t2);
	tmax = wetlift( p2, t2, 1000.0);
	pmax = lower;

	for ( i = lptr; i <= uptr; i++) {
	  if (qc(sndg[i][idxtd])) {
	    /* ----- Calculate every level that reports a dwpt ----- */
	    pres = sndg[i][idxp];
	    temp = sndg[i][idxt];
	    dwpt = sndg[i][idxtd];
	    drylift(pres, temp, dwpt, &p2, &t2);
	    t1 = wetlift(p2, t2, 1000.0);
	    if (t1 > tmax) {
	       tmax = t1;
	       pmax = pres;
	    }
	  }
	}

	/* ----- Finish with interpolated top layer ----- */
	drylift(upper, i_temp(upper, I_PRES), i_dwpt(upper, I_PRES), &p2, &t2);
	t1 = wetlift( p2, t2, 1000);
	if (t1 > tmax)
	  pmax = sndg[i][idxp];

	*param = pmax;

	return *param;
}

	
float ehi(float cape, float hel)
	/*************************************************************/
	/*  EHI                                                      */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Energy-Helicity Index.                    */
	/*************************************************************/
{
	float param;

	if (!qc(cape) || !qc(hel))
	  return RMISSD;

	return ((cape * hel) / 160000.0);
}


float ThetaE_diff(float *param)
	/*************************************************************/
	/*  THETAE_DIFF                                              */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Finds the maximum and minimum theta-e values in the      */
	/*  sounding below 500mb, and returns the difference.        */
	/*                                                           */
	/*  lower       =  Bottom level of layer (mb)  [ -1=SFC]     */
	/*  upper       =  Top level of layer (mb)     [ -1=TOP]     */
	/*************************************************************/
{
	short i, idxt, idxtd, idxp;
	float maxe = -999.0, mine = 999.0, the;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp  = getParmIndex("PRES");
	idxtd = getParmIndex("DWPT");
	idxt  = getParmIndex("TEMP");

	if (idxp == -1 || idxt == -1 || idxtd == -1)
	  return RMISSD;

	for (i = sfc()+1; i < numlvl; i++) {
	  if (qc(sndg[i][idxtd]) && qc(sndg[i][idxt])) {
	    the = thetae(sndg[i][idxp], sndg[i][idxt], sndg[i][idxtd]);
	    if (the > maxe)
	      maxe = the;
	    if (the < mine)
	      mine = the;
	    if (sndg[i][idxp] < 500.0)
	      break;
	  }
	}

	return (maxe - mine);
}


float Rogash_QPF(float *param)
	/*************************************************************/
	/*  Rogash_Rate                                              */
	/*  John Hart  SPC Norman OK                                 */
	/*                                                           */
	/*  Computes an approximate cellular convective precipitation*/
	/*  rate (expressed in inches per hour).                     */
	/*  Based on research by Joe Rogash.			     */
	/*                                                           */
	/*  *param      =  1 hour rain rate (in/hr)		     */
	/*************************************************************/
{
	short idxp;
	float  q1, q2, q3, q4, q5, x1=0;
	float  sfcpres, sfctemp, sfcdwpt;
	float  rog_updraft, rog_water;
	Parcel pcl;

	*param = RMISSD;

	if (!sndg)
	  return RMISSD;

	idxp = getParmIndex("PRES");
	if (idxp == -1)
	  return RMISSD;

	/* Lift a MOST UNSTABLE Parcel */	
	sfcpres = unstbl_lvl( &sfcpres, sndg[sfc()][idxp], -1 );
	sfctemp = i_temp(sfcpres, I_PRES);
	sfcdwpt = i_dwpt(sfcpres, I_PRES);
	x1 = parcel(-1, -1, sfcpres, sfctemp, sfcdwpt, &pcl);

	if (pcl.bplus <= 0)
	  return 0.0;

	/* Begin by determining theoretical updraft strength */
	rog_updraft = sqrt(2*pcl.bplus);

	/* Modulate updraft strength by shear */ 
	/* wind_shear( -1, i_pres(msl(6000)), &q2, &q3, &q4, &q5); */
	/* rog_updraft *= (kt_to_mps(q5) / 10) * .25; */
	rog_updraft *= 0.25;

	/* Now calculate PW density */
	rog_water = precip_water( &x1, -1, pcl.lfcpres );
	rog_water /= (i_hght(pcl.elpres, I_PRES) - i_hght(pcl.lfcpres, I_PRES));

	*param = rog_updraft * rog_water * 3600.0;

	return *param;
}

void setdcape_entrain(float value)
{
	dcape_entrain = value;
}

float getdcape_entrain(void)
{
	return dcape_entrain;
}

float dcape(float *level, float *drtemp)
	/*************************************************************/
	/*  DCAPE    						     */ 
	/*  John Hart  SPC Norman OK                                 */
	/*                                                           */
	/*  Computes downdraft instability using a modified          */
	/*  DCAPE routine.  Method determines the min ThetaE         */
	/*  in the lowest 400mb, and descends the parcel dry-        */
	/*  adiabatically to the surface.  Resulting value in        */
	/*  J/kg.                                                    */
	/*                                                           */
	/*  Input is entrainment value (-1=default)                  */
	/*  (.2 would be 20% per km)                                 */
	/*                                                           */
	/*  Returns value (J/kg).                                    */
	/*  level = pressure (mb) of layer being descended.          */
	/*  drtemp = Downrush temperature at surface (C).            */
	/*                                                           */
	/*************************************************************/
{
	short i, lptr, uptr, p5, idxt, idxtd, idxp, idxz;
        float te1, pe1, te2, pe2, h1, h2, lyre, tdef1, tdef2;
        float te3, pe3, h3, tp1, tp2, tp3, lyrlast, upper;
        float tote, ix1, mine, minep, ent2;

	if (!sndg)
	  return RMISSD;

	*level = RMISSD;
	*drtemp = RMISSD;

	idxp  = getParmIndex("PRES");
	idxz  = getParmIndex("HGHT");
	idxtd = getParmIndex("DWPT");
	idxt  = getParmIndex("TEMP");

	if (idxp == -1 || idxz == -1 || idxt == -1 || idxtd == -1)
	  return RMISSD;

	if ((int)dcape_entrain == -1)
	  dcape_entrain = ENTRAIN_DEFAULT;
        
	/* ----- Find highest observation in layer ----- */
        i=numlvl-1;
        while(sndg[i][idxp] < sndg[sfc()][idxp]-400.0) { i--;}
        p5 = i;
        if (sndg[i][idxp] == sndg[sfc()][idxp]-400.0) { p5--; }

	/* ----- Find min ThetaE layer ----- */
	mine=1000.0; minep=-999.0;
	for(i=0;i<p5;i++) {
	   if(qc(sndg[i][idxtd]) && (qc(i_temp(sndg[i][idxp]+100, I_PRES)))) {
	      Mean_thetae(&ix1, sndg[i][idxp], sndg[i][idxp]-100);
	      if (qc(ix1) && (ix1 < mine)) {
	        mine=ix1; minep=sndg[i][idxp]-50.0;
	      }
	   }
	}

	if (minep < 0)
	  return RMISSD;

	upper = minep;
	
        /* ----- Find highest observation in layer ----- */
        i=numlvl-1;
        while(sndg[i][idxp] < upper) { i--;}
        uptr = i;
        if (sndg[i][idxp] == upper) { uptr--; }

	/* ----- Define parcel starting point ----- */
	tp1 = wetbulb(upper, i_temp(upper, I_PRES), i_dwpt(upper, I_PRES)); 
	pe1 = upper;
        te1 = i_temp(pe1, I_PRES);
        h1 =  i_hght(pe1, I_PRES);
	tote = 0;
	lyre = 0;

	for(i=uptr;i>=sfc();i--) {
	   pe2 = sndg[i][idxp];
	   te2 = sndg[i][idxt];
	   h2  = sndg[i][idxz];
	   tp2 = wetlift(pe1, tp1, pe2);

	   /* Account for Entrainment */
	   /* dcape_entrain is global var - (.2 would be 20% per km) */
	   ent2 = dcape_entrain * ((h1 - h2) / 1000.0);
	   tp2 = tp2 + ((te2 - tp2) * ent2);

	   if(qc(te1) && qc(te2)) {
	   	tdef1 = (tp1 - te1) / (te1 + 273.15);
           	tdef2 = (tp2 - te2) / (te2 + 273.15);
           	lyrlast = lyre;
           	lyre = 9.8 * (tdef1 + tdef2) / 2.0 * (h2 - h1);
	   	tote += lyre;
	   }

	   pe1 = pe2;
	   te1 = te2;
	   h1 = h2;
	   tp1 = tp2;
	}

	*drtemp = tp2;
	*level = upper;
	return tote;
}


float mean_mixratio(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_MIXRATIO                                            */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Mean Mixing Ratio from data in            */
	/*  SNDG array within specified layer.                       */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned mean mixing ratio (g/kg)         */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=SFC-100mb]*/
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr;
	float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
	short pIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tdIndex = getParmIndex("DWPT");

	if (!sndg || pIndex == -1 || tdIndex == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (lower == -1)
	  lower = sndg[sfc()][pIndex];
	if (upper == -1)
	  upper = sndg[sfc()][pIndex] - 100.0;

	/* ----- Make sure this is a valid layer ----- */
	if (!qc(i_temp(upper, I_PRES)))
	  *param = RMISSD;
	if (!qc(i_temp(lower, I_PRES)))
	  lower = i_pres(sfc());

	/* ----- Find lowest observation in layer ----- */
	i = 0;
	while (sndg[i][pIndex] > lower)
	  i++;
	while (!qc(sndg[i][tdIndex]))
	  i++;
	lptr = i;
	if (sndg[i][pIndex] == lower)
	  lptr++;

	/* ----- Find highest observation in layer ----- */
	i=numlvl-1;
	while (sndg[i][pIndex] < upper)
	  i--;
	uptr = i;
	if (sndg[i][pIndex] == upper)
	  uptr--;

	/* ----- Start with interpolated bottom layer ----- */
	dp1 = i_dwpt(lower, I_PRES);
	p1 = lower;
	num = 1;

	totd = 0;
	totp = 0;
	for (i=lptr; i <= uptr; i++) {
	  if (qc(sndg[i][tdIndex])) {
	    /* ----- Calculate every level that reports a dwpt ----- */
	    dp2 = sndg[i][tdIndex];
	    p2 = sndg[i][pIndex];
	    dbar = (dp1 + dp2) / 2.0;
	    pbar = (p1 + p2) / 2.0;
	    totd = totd + dbar;
	    totp = totp + pbar;
	    dp1 = dp2;
	    p1 = p2;
	    num++;
	  }
	}

	/* ----- Finish with interpolated top layer ----- */
	dp2 = i_dwpt(upper, I_PRES);
	p2 = upper;
	dbar = (dp1 + dp2) / 2.0;
	pbar = (p1 + p2) / 2.0;
	totd = totd + dbar;
	totp = totp + pbar;
	*param = mixratio(totp/num, totd/num);

	return *param;
}


float mean_relhum(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_RELHUM                                              */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Mean Relative Humidity from data in       */
	/*  SNDG array within specified layer.                       */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned mean relative Humidity (%)       */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=TOP]      */
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr;
	float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
	float t1, t2, tbar, tott, dum;
	short pIndex, tIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tdIndex = getParmIndex("DWPT");
	tIndex = getParmIndex("TEMP");

	if (!sndg || numlvl < 1 || pIndex == -1 || tdIndex == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (lower == -1)
	  lower = sndg[sfc()][pIndex];
	if (upper == -1)
	  upper = sndg[numlvl - 1][pIndex];

	/* ----- Make sure this is a valid layer ----- */
	while (!qc(i_dwpt(upper, I_PRES)) && (upper < lower))
	  upper += 50.0;
	if (!qc(i_temp(lower, I_PRES)))
	  lower = i_pres(sfc());

	if (upper >= lower) {
	   *param = RMISSD;
	   return *param;
	}

	   /* ----- Find lowest observation in layer ----- */
	   i = 0;
	   while (sndg[i][pIndex] > lower)
	     i++;
	   while (!qc(sndg[i][tdIndex]))
	     i++;
	   lptr = i;
	   if (sndg[i][pIndex] == lower) 
	     lptr++;

	   /* ----- Find highest observation in layer ----- */
	   i=numlvl-1;
	   while (sndg[i][pIndex] < upper)
	     i--;
	   uptr = i;
	   if (sndg[i][pIndex] == upper) 
	     uptr--;

	   /* ----- Start with interpolated bottom layer ----- */
	   t1  = i_temp(lower, I_PRES);
	   dp1 = i_dwpt(lower, I_PRES);
	   p1 = lower;
	   num = 1;

	   totd = 0;
	   totp = 0;
	   tott = 0;
	   for (i=lptr; i <= uptr; i++) {
	     if (qc(sndg[i][tdIndex])) {
	       /* ----- Calculate every level that reports a dwpt ----- */
	       dp2 = sndg[i][tdIndex];
	       t2  = sndg[i][tIndex];
	       p2 = sndg[i][pIndex];
	       tbar = (t1 + t2) / 2.0;
	       dbar = (dp1 + dp2) / 2.0;
	       pbar = (p1 + p2) / 2.0;
	       totd += dbar;
	       totp += pbar;
	       tott += tbar;
	       dp1 = dp2;
	       p1 = p2;
	       t1 = t2;
	       num++;
	     }
	   }

	   /* ----- Finish with interpolated top layer ----- */
	   if (qc(i_dwpt(upper, I_PRES))) {
	     dp2 = i_dwpt(upper, I_PRES);
	     t2  = i_temp(upper, I_PRES);
	     p2 = upper;
	     tbar = (t1 + t2) / 2.0;
	     dbar = (dp1 + dp2) / 2.0;
	     pbar = (p1 + p2) / 2.0;
	     tott += tbar;
	     totd += dbar;
	     totp += pbar;
	   }

	   *param = 100.0 * mixratio(totp/num, totd/num) / 
	                    mixratio(totp/num, tott/num);

	return *param;
}


float mean_dwpt(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_DWPT                                                */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Mean Dew Point from data in               */
	/*  SNDG array within specified layer.                       */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned mean dew point (c)               */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=SFC-100mb]*/
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr;
	float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
	short pIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tdIndex = getParmIndex("DWPT");

	if (!sndg || numlvl < 1 || pIndex == -1 || tdIndex == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if( lower == -1) { lower = sndg[sfc()][pIndex]; }
	if( upper == -1) { upper = sndg[sfc()][pIndex] - 100; }

	/* ----- Make sure this is a valid layer ----- */
	if( !qc( i_temp(upper, I_PRES))) { *param = RMISSD; }
	if( !qc( i_temp(lower, I_PRES))) { lower = i_pres( sfc() ); }

	   /* ----- Find lowest observation in layer ----- */
	   i = 0;
	   while( sndg[i][pIndex] > lower)  { i++; }
	   while ( !qc(sndg[i][tdIndex]) ) { i++; }
	   lptr = i;
	   if( sndg[i][pIndex] == lower ) { lptr++; }

	   /* ----- Find highest observation in layer ----- */
	   i=numlvl-1;
	   while( sndg[i][pIndex] < upper) { i--;}
	   uptr = i;
	   if( sndg[i][pIndex] == upper ) { uptr--; }

	   /* ----- Start with interpolated bottom layer ----- */
	   dp1 = i_dwpt(lower, I_PRES);
	   p1 = lower;
	   num = 1;

	   totd = 0;
	   totp = 0;
	   for( i = lptr; i <= uptr; i++)
	      {
	      if( qc(sndg[i][tdIndex]) )
		 {
		 /* ----- Calculate every level that reports a dwpt ----- */
		 dp2 = sndg[i][tdIndex];
		 p2 = sndg[i][pIndex];
		 dbar = (dp1 + dp2) / 2;
		 pbar = (p1 + p2)/2;
		 totd = totd + dbar;
		 totp = totp + pbar;
		 dp1 = dp2;
		 p1 = p2;
		 num += 1;
		 }
	      }

	   /* ----- Finish with interpolated top layer ----- */
	   dp2 = i_dwpt(upper, I_PRES);
	   p2 = upper;
	   dbar = (dp1 + dp2) / 2;
	   pbar = (p1 + p2) / 2;
	   totd = totd + dbar;
	   totp = totp + pbar;
	   *param = totd/num;

	return *param;
}


float mean_theta(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_THETA                                               */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Mean Potential temperature of the         */
	/*  SNDG array within specified layer.                       */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned mean theta (c)                   */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=SFC-100mb]*/
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr;
	float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
	short pIndex, tIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tIndex = getParmIndex("TEMP");

	if (!sndg || numlvl < 1 || pIndex == -1 || tIndex == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if (lower == -1) { lower = sndg[sfc()][pIndex]; }
	if (upper == -1) { upper = sndg[sfc()][pIndex] - 100.0; }
	/* ----- Make sure this is a valid layer ----- */
	if (!qc(i_temp(upper, I_PRES))) { *param = RMISSD; }
	if (!qc(i_temp(lower, I_PRES))) { lower = i_pres(sfc()); }

	   /* ----- Find lowest observation in layer ----- */
	   i = 0;
	   while (sndg[i][pIndex] > lower)  { i++; }
	   while (!qc(sndg[i][tIndex]) ) { i++; }
	   lptr = i;
	   if (sndg[i][pIndex] == lower ) { lptr++; }

	   /* ----- Find highest observation in layer ----- */
	   i=numlvl-1;
	   while (sndg[i][pIndex] < upper) { i--;}
	   uptr = i;
	   if (sndg[i][pIndex] == upper) { uptr--; }

	   /* ----- Start with interpolated bottom layer ----- */
	   dp1 = theta( lower, i_temp(lower, I_PRES), 1000.0);
	   p1 = lower;
	   num = 1;

	   totd = 0;
	   totp = 0;
	   for( i = lptr; i <= uptr; i++) {
	      if( qc(sndg[i][tIndex]) ) {
		 /* ----- Calculate every level that reports a temp ----- */
		 dp2 = theta( sndg[i][pIndex], sndg[i][tIndex], 1000.0);
		 p2 = sndg[i][pIndex];
		 dbar = (dp1 + dp2) / 2.0;
		 pbar = (p1 + p2)/2.0;
		 totd = totd + dbar;
		 totp = totp + pbar;
		 dp1 = dp2;
		 p1 = p2;
		 num++;
		 }
	   }

	   /* ----- Finish with interpolated top layer ----- */
	   dp2 = theta(upper, i_temp(upper, I_PRES), 1000.0);
	   p2 = upper;
	   dbar = (dp1 + dp2) / 2.0;
	   pbar = (p1 + p2) / 2.0;
	   totd = totd + dbar;
	   totp = totp + pbar;
	   *param = totd/num;

	return *param;
}


float Mean_WBtemp(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_WBTEMP                                              */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Computes the average Wetbulb temperature between the     */
	/*  two given levels (lower,upper)			     */
	/*                                                           */
	/*  lower       =  Bottom level of layer (mb)  [ -1=SFC]     */
	/*  upper       =  Top level of layer (mb)     [ -1=WBZ]     */
	/*************************************************************/
{
        short i, okl, oku, lptr, uptr;
        float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp, ix1;
	short pIndex, tIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tIndex = getParmIndex("TEMP");
	tdIndex = getParmIndex("DWPT");

	if (!sndg || numlvl < 1 || pIndex == -1 || tIndex == -1 || 
	    tdIndex == -1)
	  return RMISSD;	

        /* ----- See if default layer is specified ----- */
        if ( lower == -1) { lower = sndg[sfc()][pIndex]; }
        if ( upper == -1) { upper = wb_lvl(0, &ix1); }

        /* ----- Make sure this is a valid layer ----- */
        if ( !qc(i_dwpt(upper, I_PRES))) { *param = RMISSD; }
        if ( !qc(i_dwpt(lower, I_PRES))) { lower = i_pres( sfc() ); }


           /* ----- Find lowest observation in layer ----- */
           i = 0;
           while( sndg[i][pIndex] > lower)  { i++; }
           while ( !qc(sndg[i][tIndex]) ) { i++; }
           lptr = i;
           if( sndg[i][pIndex] == lower ) { lptr++; }

           /* ----- Find highest observation in layer ----- */
           i=numlvl-1;
           while( sndg[i][pIndex] < upper) { i--;}
           uptr = i;
           if( sndg[i][pIndex] == upper ) { uptr--; }

           /* ----- Start with interpolated bottom layer ----- */
           dp1 = wetbulb(lower, i_temp(lower, I_PRES), i_dwpt(lower, I_PRES));
           p1 = lower;
           num = 1;

           totd = 0;
           totp = 0;
           for( i = lptr; i <= uptr; i++) {
              if( qc(sndg[i][tdIndex]) ) {
                 /* ----- Calculate every level that reports a temp ----- */
                 dp2 = wetbulb(sndg[i][pIndex], sndg[i][tIndex], 
	                       sndg[i][tdIndex]);
                 p2 = sndg[i][pIndex];
                 dbar = (dp1 + dp2) / 2.0;
                 pbar = (p1 + p2)/2.0;
                 totd = totd + dbar;
                 totp = totp + pbar;
                 dp1 = dp2;
                 p1 = p2;
                 num++;
              }
           }

           /* ----- Finish with interpolated top layer ----- */
           dp2 = wetbulb( upper, i_temp(upper, I_PRES), i_dwpt(upper, I_PRES));
           p2 = upper;
           dbar = (dp1 + dp2) / 2.0;
           pbar = (p1 + p2) / 2.0;
           totd = totd + dbar;
           totp = totp + pbar;
           *param = totd/num;

        return *param;
}


float Mean_thetae(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_THETAE                                              */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Computes the average ThetaE temperature between the      */
	/*  two given levels (lower,upper)			     */
	/*                                                           */
	/*  lower       =  Bottom level of layer (mb)  [ -1=WBZ+50]  */
	/*  upper       =  Top level of layer (mb)     [ -1=WBZ-50]  */
	/*************************************************************/
{
        short i, okl, oku, lptr, uptr;
        float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp, ix1;
	short pIndex, tIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tIndex = getParmIndex("TEMP");
	tdIndex = getParmIndex("DWPT");

	if (!sndg || numlvl < 1 || pIndex == -1 || tIndex == -1 || 
	    tdIndex == -1)
	  return RMISSD;

        /* ----- See if default layer is specified ----- */
        if (lower == -1) { lower = wb_lvl(0, &ix1)+50.0; }
        if (upper == -1) { upper = wb_lvl(0, &ix1)-50.0; }

        /* ----- Make sure this is a valid layer ----- */
        if (!qc( i_dwpt(upper, I_PRES))) { *param = RMISSD; }
        if (!qc( i_dwpt(lower, I_PRES))) { lower = i_pres(sfc()); }

           /* ----- Find lowest observation in layer ----- */
           i = 0;
           while( sndg[i][pIndex] > lower)  { i++; }
           while ( !qc(sndg[i][tIndex]) ) { i++; }
	   lptr = i;
           if( sndg[i][pIndex] == lower ) { lptr++; }

           /* ----- Find highest observation in layer ----- */
           i=numlvl-1;
           while( sndg[i][pIndex] < upper) { i--;}
           uptr = i;
           if( sndg[i][pIndex] == upper ) { uptr--; }

           /* ----- Start with interpolated bottom layer ----- */
           dp1 = thetae(lower, i_temp(lower, I_PRES), i_dwpt(lower, I_PRES));
           p1 = lower;
           num = 1;

           totd = 0;
           totp = 0;
           for( i = lptr; i <= uptr; i++)
              {
              if( qc(sndg[i][tdIndex]) )
                 {
                 /* ----- Calculate every level that reports a temp ----- */
                 dp2 = thetae(sndg[i][pIndex], sndg[i][tIndex], 
	                      sndg[i][tdIndex]);
                 p2 = sndg[i][pIndex];
                 dbar = (dp1 + dp2) / 2.0;
                 pbar = (p1 + p2) / 2.0;
                 totd = totd + dbar;
                 totp = totp + pbar;
                 dp1 = dp2;
                 p1 = p2;
                 num++;
                 }
              }

           /* ----- Finish with interpolated top layer ----- */
           dp2 = thetae( upper, i_temp(upper, I_PRES), i_dwpt(upper, I_PRES));
           p2 = upper;
           dbar = (dp1 + dp2) / 2.0;
           pbar = (p1 + p2) / 2.0;
           totd = totd + dbar;
           totp = totp + pbar;
           *param = totd/num;

        return *param;
}
	

float mean_temp(float *param, float lower, float upper)
	/*************************************************************/
	/*  MEAN_TEMP                                                */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Calculates the Mean Temperature from data in             */
	/*  SNDG array within specified layer.                       */
	/*  Value is returned both as (param) and as a RETURN;.      */
	/*                                                           */
	/*  *param      =  Returned mean temp (c)                    */
	/*  lower       =  Bottom level of layer (mb) [ -1=SFC]      */
	/*  upper       =  Top level of layer (mb)    [ -1=SFC-100mb]*/
	/*************************************************************/
{
	short i, okl, oku, lptr, uptr;
	float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
	short pIndex, tIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tIndex = getParmIndex("TEMP");

	if (!sndg || numlvl < 1 || pIndex == -1 || tIndex == -1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if ( lower == -1) { lower = sndg[sfc()][pIndex]; }
	if ( upper == -1) { upper = sndg[sfc()][pIndex] - 100.0; }

	/* ----- Make sure this is a valid layer ----- */
	if ( !qc(i_temp(upper, I_PRES))) { *param = RMISSD; }
	if ( !qc(i_temp(lower, I_PRES))) { lower = i_pres( sfc() ); }

	   /* ----- Find lowest observation in layer ----- */
	   i = 0;
	   while( sndg[i][pIndex] > lower)  { i++; }
	   while ( !qc(sndg[i][tIndex]) ) { i++; }
	   lptr = i;
	   if( sndg[i][pIndex] == lower ) { lptr++; }

	   /* ----- Find highest observation in layer ----- */
	   i=numlvl-1;
	   while( sndg[i][pIndex] < upper) { i--;}
	   uptr = i;
	   if( sndg[i][pIndex] == upper ) { uptr--; }

	   /* ----- Start with interpolated bottom layer ----- */
	   dp1 = i_temp(lower, I_PRES);
	   p1 = lower;
	   num = 1;

	   totd = 0;
	   totp = 0;
	   for( i = lptr; i <= uptr; i++) {
	      if( qc(sndg[i][tIndex]) ) {
		 /* ----- Calculate every level that reports a temp ----- */
		 dp2 = sndg[i][tIndex];
		 p2 = sndg[i][pIndex];
		 dbar = (dp1 + dp2) / 2.0;
		 pbar = (p1 + p2) / 2.0;
		 totd = totd + dbar;
		 totp = totp + pbar;
		 dp1 = dp2;
		 p1 = p2;
		 num++;
		 }
	   }

	   /* ----- Finish with interpolated top layer ----- */
	   dp2 = i_temp(upper, I_PRES);
	   p2 = upper;
	   dbar = (dp1 + dp2) / 2.0;
	   pbar = (p1 + p2) / 2.0;
	   totd = totd + dbar;
	   totp = totp + pbar;
	   *param = totd/num;

	return *param;
}


/*
THIS ROUTINE DOES NOT LOOK FINISHED

I guess it is though. It's being used.
*/

float uncapped_cape(float *param, float *level, float maxcin)
        /*************************************************************/
        /*  UNCAPPED_CAPE                                            */
        /*  John Hart  SPC OUN                                       */
        /*                                                           */
        /*  Computes the cape at the most unstable level below 500mb */
        /*  that is not capped greater that maxcin                   */
        /*                                                           */
        /*  maxcin      =  Convective inhibition threshold (j/kg)    */
	/*  level       =  Level of max CAPE (mb)                    */
        /*  *param      =  Returned CAPE (j/kg)                      */
        /*************************************************************/
{
	int    i;
	float  maxcape, maxcapelevel, ix1, t5;
	Parcel pcl;
	short pIndex, tIndex, tdIndex;

	*param = RMISSD;

	pIndex = getParmIndex("PRES");
	tIndex = getParmIndex("TEMP");
	tdIndex = getParmIndex("DWPT");

	if (!sndg || pIndex == -1 || tIndex == -1 || tdIndex == -1)
	  return RMISSD;

	maxcape = 0;
	maxcapelevel = 0;
	/* Loop through each layer below 500mb and compute CAPE */ 
        /* If 500mb T is missing (i.e. a short sounding) then   */
        /* return uncapped_cape as missing                      */

        if ( !qc(t5)) {
          *param = RMISSD;
/*          printf( "Missing 500mb Temp, not computing uncapped_cape\n"); */
        }
        else {
          for (i=1; sndg[i][pIndex]>500.0; i++) {
            ix1 = parcel(-1, -1, sndg[i][pIndex], sndg[i][tIndex],
                    sndg[i][tdIndex], &pcl);
            if (pcl.bplus > maxcape) {
              if ((pcl.bminus >= maxcin) &&
                  ((sndg[i][tIndex] - sndg[i][tdIndex]) <= 5)) {
                maxcape = pcl.bplus;
                maxcapelevel = pcl.lplpres;
              }
            }
          }
        }
}


/*
 * Routine to find the lowest observation in a sounding
 *
 * bottom - minimum level to search to (mb for pressure, meters for height)
 * itype - I_PRES for pressure and I_HGHT for height
 * idx - the index of the variable you're looking for
 *
 * -mkay
 */
short findlowobindex(float bottom, short itype, short idx)
{
	short i=0, j, lptr=-1;

	if (!sndg || idx >= ndsetparms || idx == -1 || !qc(bottom))
	  return -1;

	if (itype == I_PRES) {
	  j = getParmIndex("PRES");
	  if (j == -1)
	    return -1;

	  /*
	   * Find the first pressure level above the bottom
	   */
	  while(sndg[i][j] > bottom) {
	    i++;
	    if (i == (numlvl-1)) {
	      fprintf(stderr, "Warning: findlowobindex: Breaking out early.\n");
	      break;
	    }
	  }
	  /*
	   * Now we start from that level and find the first level where
	   * we have valid data
	   */
	  while(!qc(sndg[i][idx])) {
	    i++;
	    if (i >= numlvl) {
	      fprintf(stderr, "Warning: findlowobindex: No good data found.\n");
	      return -1;
	    }
	  }
	  lptr = i;

	  if (sndg[i][j] == bottom)
	    lptr++;
	}
	else if (itype == I_HGHT) {
	  j = getParmIndex("HGHT");
	  if (j == -1)
	    return -1;

          while(sndg[i][j] < bottom) {
	    i++;
	    if (i == (numlvl-1)) {
	      fprintf(stderr, "Warning: findlowobindex: Breaking out early.\n");
	      break;
	    }
	  }
	  while(!qc(sndg[i][idx])) {
	    i++;
	    if (i >= numlvl) {
	      fprintf(stderr, "Warning: findlowobindex: No good data found.\n");
	      return -1;
	    }
	  }
          lptr = i;

	  if (sndg[i][j] == bottom)
	    lptr++;
	}

	return lptr;
}


/*
 * Routine to find the highest observation in a sounding
 *
 * top - maximum level to search to (mb for pressure, meters for height)
 * itype - I_PRES for pressure and I_HGHT for height
 * idx - the index of the variable you're looking for
 *
 * -mkay
 */
short findhighobindex(float top, short itype, short idx)
{
	short i, j, uptr=-1;

	if (!sndg || idx >= ndsetparms || idx == -1 || !qc(top))
	  return -1;

	if (itype == I_PRES) {
	  j = getParmIndex("PRES");
	  if (j == -1)
	    return -1;

          i=numlvl-1;
          while(sndg[i][j] < top) {
	    i--;
	    if (i == 0) {
	      fprintf(stderr,
	      "Warning: findhighobindex: Breaking out early.\n");
	      break;
	    }
	  }
	  while(!qc(sndg[i][idx])) {
	    i--;
	    if (i <= 0) {
	      fprintf(stderr,
	      "Warning: findhighobindex: Breaking out early.\n");
	      return -1;
	    }
	  }
          uptr = i;

          if (sndg[i][j] == top)
	    uptr--;
	}
	else if (itype == I_HGHT) {
	  j = getParmIndex("HGHT");
	  if (j == -1)
	    return -1;

          i=numlvl-1;
          while(sndg[i][j] > top) {
	    i--;
	    if (i <= 0) {
	      fprintf(stderr,
	      "Warning: findhighobindex: Breaking out early.\n");
	      break;
	    }
	  }
	  while(!qc(sndg[i][idx])) {
	    i--;
	    if (i <= 0) {
	      fprintf(stderr,
	      "Warning: findhighobindex: Breaking out early.\n");
	      return -1;
	    }
	  }
          uptr = i;

          if (sndg[i][j] == top)
	    uptr--;
	}

	return uptr;
}


float scp(float stdir, float stspd)
/***************************************************************/
/*                                                             */
/*  Computes the Supercell Composite Parameter                 */
/*  Updated to 2005 WAF version - RLT 1/17/05                  */
/*                                                             */
/***************************************************************/
        {
        float ix1, ix2, ix3, ix4, lpl, el, depth, eshear, esrh, ebot, etop, fudge, cape, scp_new;
	float eshear40, eshear50, eshear60;
	int idxp;
        struct _parcel pcl;

	idxp = getParmIndex("PRES");

        /* How likely are supercell storms? */
        /* scp = 6km Shear * 3keh * mucp / 6000000 */
        /* scp > 1 indicates some supercell threat.  scp > 3 is a strong signal */

        ix1 = parcel( -1, -1, lplvals.pres, lplvals.temp, lplvals.dwpt, &pcl);

	lpl = agl(i_hght(pcl.lplpres, I_PRES));
	el = agl(i_hght(pcl.elpres, I_PRES));
	depth = (el - lpl);

	wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.4))), &ix1, &ix2, &ix3, &ix4);
	eshear40 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.5))), &ix1, &ix2, &ix3, &ix4);
        eshear50 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.6))), &ix1, &ix2, &ix3, &ix4);
        eshear60 = ix4;

	if ((eshear40 >= eshear50) && (eshear40 >= eshear60)) eshear=eshear40;
        else if ((eshear50 >= eshear40) && (eshear50 >= eshear60)) eshear=eshear50;
        else if ((eshear60 >= eshear40) && (eshear60 >= eshear50)) eshear=eshear60; 

	if (eshear < 0) { 
		wind_shear( i_pres(msl(0)), i_pres(msl(6000)), &ix1, &ix2, &ix3, &ix4);
		eshear = ix4;
		}
		
/*	printf("\neshear (for SCP) = %.1f\n", eshear); */

	effective_inflow_layer( 100, -250, &ix1, &ix2);
	ebot = ix1;
	etop = ix2;	

        esrh = helicity( agl(i_hght(ebot, I_PRES)), agl(i_hght(etop, I_PRES)), stdir, stspd, &ix2, &ix3);
/*	printf("\nesrh = %.1f\n", esrh); */

        cape   = pcl.bplus;
/*	printf("\ncape = %.1f\n", cape); */

	if (!qc(cape)) return -9999;

	if (eshear < 20 ) eshear = 0;
	if (eshear > 40 ) eshear = 1;
	else
		eshear = (eshear / 40 );
	if (esrh < -998) esrh = 0;

        scp_new = (eshear * (esrh/50) * (cape/1000));
/*	printf("\nscp_new = %.1f\n", scp_new); */
        return scp_new;
        }

float sigtorn(float stdir, float stspd)
/***************************************************************/
/*                                                             */
/*  	WAF 2005 Significant Tornado Parameter                 */
/*	Rich Thompson SPC OUN				       */	
/*                                                             */
/***************************************************************/
        {
        float ix1, ix2, ix3, ix4, eshear, esrh, mlcape, lclh, stp, cape3km, lpl, el, depth, ebot, etop;
	float eshear40, eshear50, eshear60;
	int idxp;
        struct _parcel pcl;

        idxp = getParmIndex("PRES");

        /* How likely are significant tornadoes? */
        /* scp = (6km Shear/40) * (1keh/150) * (mucp/1000) * (lcl hgt / 2000m) * (cin / 100) *(3kmcape / 75) */

        ix1 = parcel( -1, -1, lplvals.pres, lplvals.temp, lplvals.dwpt, &pcl);

        lpl = agl(i_hght(pcl.lplpres, I_PRES));
        el = agl(i_hght(pcl.elpres, I_PRES));
        depth = (el - lpl);

        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.4))), &ix1, &ix2, &ix3, &ix4);
        eshear40 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.5))), &ix1, &ix2, &ix3, &ix4);
        eshear50 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.6))), &ix1, &ix2, &ix3, &ix4);
        eshear60 = ix4;

        if ((eshear40 >= eshear50) && (eshear40 >= eshear60)) eshear=eshear40;
        else if ((eshear50 >= eshear40) && (eshear50 >= eshear60)) eshear=eshear50;
        else if ((eshear60 >= eshear40) && (eshear60 >= eshear50)) eshear=eshear60;

        if (el < 0) {
                wind_shear( i_pres(msl(0)), i_pres(msl(6000)), &ix1, &ix2, &ix3, &ix4);
                eshear = ix4;
                }

        effective_inflow_layer( 100, -250, &ix1, &ix2);
        ebot = ix1;
        etop = ix2;

        esrh = helicity( agl(i_hght(ebot, I_PRES)), agl(i_hght(etop, I_PRES)), stdir, stspd, &ix2, &ix3);

        mlcape   = pcl.bplus;

        cape3km = pcl.cape3km;

        /* cinh   = 150 + pcl.bminus; */

        if (!qc(mlcape)) return -9999;

	lclh = agl(i_hght(pcl.lclpres, I_PRES));

        if (lclh > 2000) lclh = 0;
        else
        if (lclh <= 1000) lclh = 1;
        else
                lclh = ((2000-lclh)/1000);

	printf("\nlcl_term = %.1f\n", lclh);

        if (eshear < 25 ) eshear = 0;
        else
        if (eshear >= 60) eshear = 1.5;
        else
                eshear = (eshear/40);

        if (esrh <  -9998) esrh = 0;

        /* stp = (shr6/40) * (eh1km/100) * (cape/1000) * (cinh/125) * (lclh/1750); */
        stp = (eshear) * (esrh/150) * (mlcape/1500) * (lclh);
        if (stp < 0) stp = 0;
        return stp;
	}

float sigtorn_cin(float stdir, float stspd)
/***************************************************************/
/*                                                             */
/* 	WAF 2005 Significant Tornado Parameter (with CIN)      */
/*	Rich Thompson SPC OUN				       */ 
/*                                                             */
/***************************************************************/
        {
        float ix1, ix2, ix3, ix4, eshear, esrh, mlcape, mlcinh, lclh, stp_cin, lpl, el, depth, ebot, etop;
        float eshear40, eshear50, eshear60;
	int idxp;
        struct _parcel pcl;

        idxp = getParmIndex("PRES");

        /* How likely are significant tornadoes? */
        /* scp = (6km Shear/40) * (1keh/150) * (mucp/1000) * (lcl hgt / 2000m) * (cin / 100) *(3kmcape / 75) */

        ix1 = parcel( -1, -1, lplvals.pres, lplvals.temp, lplvals.dwpt, &pcl);

        lpl = agl(i_hght(pcl.lplpres, I_PRES));
        el = agl(i_hght(pcl.elpres, I_PRES));
        depth = (el - lpl);

        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.4))), &ix1, &ix2, &ix3, &ix4);
        eshear40 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.5))), &ix1, &ix2, &ix3, &ix4);
        eshear50 = ix4;
        wind_shear(pcl.lplpres, i_pres(msl(lpl + (depth * 0.6))), &ix1, &ix2, &ix3, &ix4);
        eshear60 = ix4;

        if ((eshear40 >= eshear50) && (eshear40 >= eshear60)) eshear=eshear40;
        else if ((eshear50 >= eshear40) && (eshear50 >= eshear60)) eshear=eshear50;
        else if ((eshear60 >= eshear40) && (eshear60 >= eshear50)) eshear=eshear60;

        if (el < 0) {
                wind_shear( i_pres(msl(0)), i_pres(msl(6000)), &ix1, &ix2, &ix3, &ix4);
                eshear = ix4;
                }

        effective_inflow_layer( 100, -250, &ix1, &ix2);
        ebot = ix1;
        etop = ix2;

        esrh = helicity( agl(i_hght(ebot, I_PRES)), agl(i_hght(etop, I_PRES)), stdir, stspd, &ix2, &ix3);

        mlcape   = pcl.bplus;

	mlcinh   = pcl.bminus;

	/* cinh   = 150 + pcl.bminus; */

	if (!qc(mlcape)) return -9999;

        if (mlcinh >= -50 ) mlcinh=1;
        else
        if (mlcinh < -200 ) mlcinh=0;
        else
                mlcinh = ((200 + mlcinh)/150);

        lclh = agl(i_hght(pcl.lclpres, I_PRES));

        if (lclh > 2000) lclh = 0;
        else
        if (lclh <= 1000) lclh = 1;
        else
                lclh = ((2000-lclh)/1000);

        if (eshear < 25 ) eshear = 0;
        else
        if (eshear >= 60) eshear = 1.5;
        else
                eshear = (eshear/40);

        if (esrh <  -9998) esrh = 0;

        /* stp = (shr6/40) * (eh1km/100) * (cape/1000) * (cinh/125) * (lclh/1750); */
        stp_cin = (eshear) * (esrh/150) * (mlcape/1500) * (lclh) * (mlcinh);

/*      printf( "mlcape=%f\nmlcinh=%f\nlclh=%f\neshear=%f\nesrh=%f\nstp_new_cin=%f\n", mlcape, mlcinh, lclh, eshear, esrh, stp_cin);
*/
	if (stp_cin < 0) stp_cin = 0;
        return stp_cin;
	}

#ifdef BLAMMO
float parcelx(float lower, float upper, float pres, float temp,
	      float dwpt, Parcel *pcl)
	/*************************************************************/
	/*  PARCELX                                                  */
	/*  John Hart  NSSFC KCMO                                    */
	/*                                                           */
	/*  Lifts specified parcel, calculating various levels and   */
	/*  parameters from data in SNDG array.  B+/B- are           */
	/*  calculated based on layer (lower-upper).  Bplus is       */
	/*  returned value and in structure.                         */
	/*                                                           */
	/*  No virtual corrections are used.                         */
	/*                                                           */
	/*  lower       =  Bottom level of layer (mb)  [ -1=SFC]     */
	/*  upper       =  Top level of layer (mb)     [ -1=TOP]     */
	/*  pres        =  LPL pressure (mb)                         */
	/*  temp        =  LPL temperature (c)                       */
	/*  dwpt        =  LPL dew point (c)                         */
	/*  pcl         =  returned _parcel structure                */
	/*************************************************************/
	{
	short i, lptr, uptr;
	float te1, pe1, te2, pe2, h1, h2, lyre, tdef1, tdef2, totp, totn;
	float te3, pe3, h3, tp1, tp2, tp3, tdef3, lyrf, lyrlast, pelast;
	float tote, totx, ls1, cap_strength, li_max, li_maxpres, bltheta, pp;
	float cap_strengthpres, tpx, ix1, blupper, pp1, pp2, dz, blmr;

	/* ----- Initialize PCL values ----- */
	pcl->lplpres = pres;
	pcl->lpltemp = temp;
	pcl->lpldwpt = dwpt;
	pcl->blayer = lower;
	pcl->tlayer = upper;
	pcl->entrain = 0;
	pcl->lclpres = -999;
	pcl->lfcpres = -999;
	pcl->elpres = -999;
	pcl->mplpres = -999;
	pcl->bplus = -999;
	pcl->bminus = -999;
	pcl->bfzl = -999;
	pcl->li5 = -999;
	pcl->li3 = -999;
	pcl->brn = -999;
	pcl->limax = -999;
	pcl->limaxpres = -999;
	pcl->cap = -999;
	pcl->cappres = -999;

	lyre = -1;
	cap_strength = -999;
	cap_strengthpres = -999;
	li_max = -999;
	li_maxpres = -999;
	totp = 0;
	totn = 0;

	if (!sndg || numlvl < 1)
	  return RMISSD;

	/* ----- See if default layer is specified ----- */
	if( lower == -1)
	   {
	   lower = sndg[sfc()][1];
	   pcl->blayer = lower;
	   }
	if( upper == -1)
	   {
	   upper = sndg[numlvl-1][1];
	   pcl->tlayer = upper;
	   }

	/* ----- Make sure this is a valid layer ----- */
	if( lower > pres )
	   {
	   lower = pres;
	   pcl->blayer = lower;
	   }
	if( !qc( i_temp( upper ))) { return -999; }
	if( !qc( i_temp( lower ))) { return -999; }

	/* ----- Begin with Mixing Layer (LPL-LCL) ----- */
	te1 = i_temp( pres );
	pe1 = lower;
	h1 =  i_hght( pe1 );
	tp1 = temp;

	drylift(pres, temp, dwpt, &pe2, &tp2);
	blupper=pe2;
	h2 =  i_hght( pe2 );
	te2 = i_temp( pe2 );
	pcl->lclpres = pe2;

        bltheta = theta(pres, i_temp(pres), 1000);
        blmr = mixratio(pres, dwpt);

        /* ----- Accumulate CINH in mixing layer below the LCL ----- */
        /* This will be done in 10mb increments, and will use the    */
        /* virtual temperature correction where possible.            */

        totn=0;
        for(pp=lower;pp>blupper;pp-=10)
           {
           pp1 = pp;
           pp2 = pp-10;
           if (pp2<blupper) pp2=blupper;

           dz = i_hght(pp2) - i_hght(pp1);
	   /* old CIN routine below LCL */	
/*         tp1 = theta(1000, bltheta, pp1);
           tp2 = theta(1000, bltheta, pp2);
           tdef1 = (tp1 - i_temp(pp1)) / (i_temp(pp1) + 273.15);
           tdef2 = (tp2 - i_temp(pp2)) / (i_temp(pp2) + 273.15);
*/
	   /* new CIN routine below LCL - RLT 3/9/05 */
           tp1 = theta(pp1, bltheta, 1000);
           tp2 = theta(pp2, bltheta, 1000);
	   tdef1 = (tp1 - theta(pp1, i_temp(pp1), i_dwpt(pp1))) / (theta(pp1, i_temp(pp1), i_dwpt(pp1)) + 273.15);
           tdef1 = (tp2 - theta(pp2, i_temp(pp2), i_dwpt(pp2))) / (theta(pp2, i_temp(pp2), i_dwpt(pp2)) + 273.15);	
           lyre = 9.8F * (tdef1 + tdef2) / 2.0F * dz;

           if (lyre<0) totn+=lyre;
           }

	if( lower > pe2 )
	   {
	   lower = pe2;
	   pcl->blayer = lower;
	   }

	/* ----- Find lowest observation in layer ----- */
	i = 0;
	while( sndg[i][1] > lower)  { i++; }
	while ( !qc(sndg[i][4]) ) { i++; }
	lptr = i;
	if( sndg[i][1] == lower ) { lptr++; }

	/* ----- Find highest observation in layer ----- */
	i=numlvl-1;
	while(sndg[i][1] < upper) { i--; }
	uptr = i;
	if( sndg[i][1] == upper ) { uptr--; }

	/* ----- Start with interpolated bottom layer ----- */
	pe1 = lower;
	h1 =  i_hght( pe1 );
	te1 = i_temp( pe1 );
	tp1 = wetlift(pe2, tp2, pe1);

	totp = 0;
	for( i = lptr; i < numlvl; i++)
	   {
	   if( qc(sndg[i][3]) )
	      {
	      /* ----- Calculate every level that reports a temp ----- */
	      pe2 = sndg[i][1];
	      h2 =  sndg[i][2];
	      te2 = i_temp( pe2 );
	      tp2 = wetlift(pe1, tp1, pe2);
	      tdef1 = (tp1 - te1) / (te1 + 273.15);
	      tdef2 = (tp2 - te2) / (te2 + 273.15);
	      lyrlast = lyre;
	      lyre = 9.8F * (tdef1 + tdef2) / 2.0F * (h2 - h1);

	      /* ----- Check for Max LI ----- */
	      if ((tp2 - te2) > li_max)
		 {
		 li_max = tp2 - te2;
		 li_maxpres = pe2;
		 }

	      /* ----- Check for Max Cap Strength ----- */
	      if ((te2 - tp2) > cap_strength)
		 {
		 cap_strength = te2 - tp2;
		 cap_strengthpres = pe2;
		 }

	      if( lyre > 0 )
		 { totp += lyre; }
	      else
		 {
		 if(pe2 > 500) { totn += lyre; }
		 }

	      tote += lyre;
	      pelast = pe1;
	      pe1 = pe2;
	      h1 = h2;
	      te1 = te2;
	      tp1 = tp2;

	      /* ----- Is this the top of given layer ----- */
	      if((i >= uptr) && ( !qc(pcl->bplus)))
		 {
		 pe3 = pe1;
		 h3 = h1;
		 te3 = te1;
		 tp3 = tp1;
		 lyrf = lyre;

		 if( lyrf > 0 )
		    {
		    pcl->bplus = totp - lyrf;
		    pcl->bminus = totn;
		    }
		 else
		    {
		    pcl->bplus = totp;
		    if(pe2 > 500)
		       { pcl->bminus = totn - lyrf; }
		    else
		       { pcl->bminus = totn; }
		    }

		 pe2 = upper;
		 h2 = i_hght( pe2 );
		 te2 = i_temp( pe2 );
		 tp2 = wetlift(pe3, tp3, pe2);
		 tdef3 = (tp3 - te3) / (te3 + 273.15);
		 tdef2 = (tp2 - te2) / (te2 + 273.15);
		 lyrf = 9.8F * (tdef3 + tdef2) / 2.0F * (h2 - h3);
		 if( lyrf > 0 )
		    { pcl->bplus += lyrf; }
		 else
		    { if(pe2 > 500) { pcl->bminus += lyrf; } }

		 if( pcl->bplus == 0 ) { pcl->bminus = 0; }
		 }

	      /* ----- Is this the freezing level ----- */
	      if((te2 <= 0) && ( !qc(pcl->bfzl)))
		 {
		 pe3 = pelast;
		 h3 = i_hght( pe3 );
		 te3 = i_temp( pe3 );
		 tp3 = wetlift(pe1, tp1, pe3);
		 lyrf = lyre;

		 if( lyrf > 0 )
		    { pcl->bfzl = totp - lyrf; }
		 else
		    { pcl->bfzl = totp; }

		 pe2 = temp_lvl( 0, &pe2);
		 h2 = i_hght( pe2 );
		 te2 = i_temp( pe2 );
		 tp2 = wetlift(pe3, tp3, pe2);
		 tdef3 = (tp3 - te3) / (te3 + 273.15);
		 tdef2 = (tp2 - te2) / (te2 + 273.15);
		 lyrf = 9.8F * (tdef3 + tdef2) / 2.0F * (h2 - h3);
		 if( lyrf > 0 )
		    { pcl->bfzl += lyrf; }
		 }

	      /* ----- LFC Possibility ----- */
	      if(( lyre >= 0 ) && ( lyrlast < 0 ))
		 {
		 tp3 = tp1;
		 te3 = te1;
		 pe2 = pe1;
		 pe3 = pelast;
		 while( i_temp( pe3 ) > wetlift(pe2, tp3, pe3) ) { pe3 -= 5; }
		 pcl->lfcpres = pe3;

		 tote = 0;
		 pcl->elpres = -999;

		 li_max = -999;

		 pcl->cap = cap_strength;
		 pcl->cappres = cap_strengthpres;
		 }

	      /* ----- EL Possibility ----- */
	      if(( lyre <= 0 ) && ( lyrlast > 0 ))
		 {
		 tp3 = tp1;
		 te3 = te1;
		 pe2 = pe1;
		 pe3 = pelast;
		 while ( i_temp( pe3 ) < wetlift(pe2, tp3, pe3) ) { pe3 -= 5; }
		 pcl->elpres = pe3;
		 pcl->mplpres = -999;
		 pcl->limax = -li_max;
		 pcl->limaxpres = li_maxpres;
		 }

	      /* ----- MPL Possibility ----- */
	      if(( (tote <= 0) && ( !qc( pcl->mplpres ) ) && ( qc( pcl->elpres ))))
		 {
		 pe3 = pelast;
		 h3 = i_hght( pe3 );
		 te3 = i_temp( pe3 );
		 tp3 = wetlift(pe1, tp1, pe3);
		 totx = tote - lyre;

		 pe2 = pelast;
		 while( totx > 0 )
		    {
		    pe2 -= 1;
		    te2 = i_temp( pe2 );
		    tp2 = wetlift(pe3, tp3, pe2);
		    h2 = i_hght( pe2 );
		    tdef3 = (tp3 - te3) / (te3 + 273.15);
		    tdef2 = (tp2 - te2) / (te2 + 273.15);
		    lyrf = 9.8F * (tdef3 + tdef2) / 2.0F * (h2 - h3);
		    totx += lyrf;

		    tp3 = tp2;
		    te3 = te2;
		    pe3 = pe2;
		    }
		 pcl->mplpres = pe2;
		 }

	      /* ----- 500mb Lifted Index ----- */
	      if( sndg[i][1] == 500 )
		 {
		 pcl->li5 = sndg[i][3] - tp1;
		 }

	      /* ----- 300mb Lifted Index ----- */
	      if( sndg[i][1] == 300 )
		 {
		 pcl->li3 = sndg[i][3] - tp1;
		 }
	      }
	   }
	/* ----- Calculate BRN if available ----- */
	pcl->brn = bulk_rich( *pcl, &ix1 );
	return pcl->bplus;
	}
#endif /* BLAMMO */

float CB_sigtor(void)
/********************************************************************/
/* Craven&Brooks SigTorn                                            */
/********************************************************************/
	{
        float ix1, ix2, ix3, ix4, shr6, shr1, cape, lclh, cbt, dcapeval;
	short idxp;
        struct _parcel pcl;
	struct _lplvalues lpl2;

        /* How likely are significant tornadoes? */
        /* CBT = (6ksh * 1ksh * mlcp) / (mllcl hgt * dcape) */
	/* units are m/s2 */

	idxp = getParmIndex("PRES");

	lpl2 = lplvals;

	/* lift a mixed parcel */
	define_parcel(4, 100);
        ix1 = parcel( -1, -1, lplvals.pres, lplvals.temp, lplvals.dwpt, &pcl);
	lplvals = lpl2;

        wind_shear(sndg[sfc()][idxp], i_pres(msl(6000)), &ix1, &ix2, &ix3, &ix4);
        shr6 = ix4 *.514;

        wind_shear(sndg[sfc()][idxp], i_pres(msl(1000)), &ix1, &ix2, &ix3, &ix4);
        shr1 = ix4 *.514;

        cape   = pcl.bplus;

	dcapeval = dcape(&ix1, &ix2);

        lclh   = agl( i_hght(pcl.lclpres, I_PRES) );

        if (!qc(cape)) return -9999;

        cbt = (cape * shr6 * shr1) / (dcapeval * lclh);
        if (cbt > 5) cbt = -999;

        return cbt;
	}

float mean_omeg(float *param, float lower, float upper)
        /*************************************************************/
        /*  MEAN_OMEG                                                */
        /*  John Hart  NSSFC KCMO                                    */
        /*                                                           */
        /*  Calculates the Mean omega from data in                   */
        /*  SNDG array within specified layer.                       */
        /*  Value is returned both as (param) and as a RETURN;.      */
        /*                                                           */
        /*  *param      =  Returned mean omega (microb/s)            */
        /*  lower       =  Bottom level of layer (mb) [ -1=700mb]    */
        /*  upper       =  Top level of layer (mb)    [ -1=500mb]    */
        /*************************************************************/
{
        short i, okl, oku, lptr, uptr;
        float num, dp1, dp2, totd, dbar, p1, p2, pbar, totp;
        short pIndex, oIndex;

        *param = RMISSD;

        pIndex = getParmIndex("PRES");
        oIndex = getParmIndex("OMEG");

        if (!sndg || numlvl < 1 || pIndex == -1 || oIndex == -1) return RMISSD;

        /* ----- See if default layer is specified ----- */
        if( lower == -1) { lower = 700; }
        if( upper == -1) { upper = 500; }

        /* ----- Make sure this is a valid layer ----- */
        if( !qc( i_temp(upper, I_PRES))) { *param = RMISSD; }
        if( !qc( i_temp(lower, I_PRES))) { lower = i_pres( sfc() ); }

           /* ----- Find lowest observation in layer ----- */
           i = 0;
           while( sndg[i][pIndex] > lower)  { i++; }
           while ( !qc(sndg[i][oIndex]) ) { i++; }
           lptr = i;
           if( sndg[i][pIndex] == lower ) { lptr++; }

           /* ----- Find highest observation in layer ----- */
           i=numlvl-1;
           while( sndg[i][pIndex] < upper) { i--;}
           uptr = i;
           if( sndg[i][pIndex] == upper ) { uptr--; }

           /* ----- Start with interpolated bottom layer ----- */
           dp1 = i_omeg(lower, I_PRES);
           p1 = lower;
           num = 1;

           totd = 0;
           totp = 0;
           for( i = lptr; i <= uptr; i++)
              {
              if( qc(sndg[i][oIndex]) )
                 {
                 /* ----- Calculate every level that reports omeg ----- */
                 dp2 = sndg[i][oIndex];
                 p2 = sndg[i][pIndex];
                 dbar = (dp1 + dp2) / 2;
                 pbar = (p1 + p2)/2;
                 totd = totd + dbar;
                 totp = totp + pbar;
                 dp1 = dp2;
                 p1 = p2;
                 num += 1;
                 }
              }

           /* ----- Finish with interpolated top layer ----- */
           dp2 = i_omeg(upper, I_PRES);
           p2 = upper;
           dbar = (dp1 + dp2) / 2;
           pbar = (p1 + p2) / 2;
           totd = totd + dbar;
           totp = totp + pbar;
           *param = totd/num;

        return *param;
}


float advection_layer(float *param, float lower, float upper)
        /*************************************************************/
        /*  ADVECTION_LAYER                                          */
        /*  John Hart  SPC OUN                                       */
        /*                                                           */
        /*  lower       =  lower end of layer (mb)                   */
        /*  upper       =  upper end of layer (mb)                   */
        /*  *param      =  Returned value (C/hr)                     */
        /*************************************************************/
{
        int    i;
        float  mw_u, mw_v, mw_drct, mw_sped, dpth;
	float  sh_u, sh_v, sh_drct, sh_sped, mean_t;
	float  ix1, advt, term1, term2, term3, con1;
        Parcel pcl;
        short pIndex, tIndex, wsIndex, wdIndex;

        *param = RMISSD;

        pIndex = getParmIndex("PRES");
        tIndex = getParmIndex("TEMP");
        wsIndex = getParmIndex("SPED");
        wdIndex = getParmIndex("DRCT");

        if (!sndg || pIndex == -1 || tIndex == -1 || wsIndex == -1 || wdIndex == -1) return RMISSD;

	/* Mean wind through the layer */
	mean_wind(lower, upper, &mw_u, &mw_v, &mw_drct, &mw_sped);
	if (mw_drct < -999) return RMISSD;

	/* Mean temperature through the layer */
	mean_t = mean_temp(&ix1, lower, upper) + 273.15;
	if (mean_t < -999) return RMISSD;

	/* Shear through the layer */
	wind_shear(lower, upper, &sh_u, &sh_v, &sh_drct, &sh_sped);
	if (sh_drct < -999) return RMISSD;

	/* Depth of the layer */
	dpth = i_hght(upper, I_PRES) - i_hght(lower, I_PRES);
	if (dpth < -999) return RMISSD;

	term1 = kt_to_mps(mw_v) * (kt_to_mps(sh_u)/dpth);
	term2 = kt_to_mps(mw_u) * (kt_to_mps(sh_v)/dpth);
	con1  = (.0004 / 9.81);						/* -f/g */
	term3 = con1 * mean_t * (term1 - term2) * 3600;

	*param = term3;
	return term3;
}
