C$PRAGMA C (UDATL)
C MODULE EILUVO
C-----------------------------------------------------------------------
C
      SUBROUTINE EILUVO
C.......................................................................
C.......................................................................
C
C     THIS ROUTINE SETS THE VALUES IN ESPEX COMMON BLOCKS FOR
C      UNIVERSAL TECHNIQUES AND ARGUMENTS FROM IGTECH AND IGARG ARRAYS
C.......................................................................
C.......................................................................
C
C
C   THIS ROUTINE WAS WRITTEN BY GERALD N. DAY .
C
C
      COMMON/HDFLTS/XHCL(2),LINPTZ,YHCL(18),LCLHCL,LTZHCL,NPDHCL,NCAHCL
C
      INCLUDE 'common/fcrunc'
      INCLUDE 'common/fctime'
      INCLUDE 'common/fctim2'
      INCLUDE 'common/ionum'
      INCLUDE 'common/fdbug'
      INCLUDE 'common/sysbug'
      INCLUDE 'common/errdat'
      INCLUDE 'common/where'
      INCLUDE 'common/etime'
      INCLUDE 'common/esprun'
      INCLUDE 'common/eswtch'
      INCLUDE 'common/eoutpt'
      INCLUDE 'common/eunit'
      INCLUDE 'common/eyrwt'
      INCLUDE 'common/fengmt'
      INCLUDE 'common/fmodft'
      INCLUDE 'common/fcary'
      INCLUDE 'clbcommon/chfdim'
      INCLUDE 'clbcommon/crwctl'
      include 'common/egentr'
      INCLUDE 'common/etsunt'
      INCLUDE 'common/eadjq'
C
      DIMENSION SBNAME(2),OPHOLD(2)
      DIMENSION IDATE(6),NAMRID(2,3),IARGS(70)
C
C    ================================= RCS keyword statements ==========
      CHARACTER*68     RCSKW1,RCSKW2
      DATA             RCSKW1,RCSKW2 /                                 '
     .$Source: /fs/hseb/ob72/rfc/ofs/src/shared_esp/RCS/eiluvo.f,v $
     . $',                                                             '
     .$Id: eiluvo.f,v 1.9 2002/10/10 16:10:04 dws Exp $
     . $' /
C    ===================================================================
C
C
      DATA BLANK/4H    /,IBLNK/4H    /,IYES/4HYES /
      DATA ICBG/4HFILU/
      DATA NAMRID/4HCGRO,4HUP  ,4HFGRO,4HUP  ,4HONES,4HEG  /
      DATA SBNAME/4HEILU,4HVO  /
C
      IF(ITRACE.GT.0)WRITE(IODBUG,600)
  600 FORMAT(1H0,'*** EILUVO ENTERED ***')
C
      DO 5 I=1,2
      OPHOLD(I)=OPNAME(I)
    5 OPNAME(I)=SBNAME(I)
      IOPHLD=IOPNUM
      IOPNUM=0
C
C            FILL SYSBUG COMMON FIRST - THEN CHECK DEBUG CODES FOR THIS
C                    ROUTINE
C
      CALL HPASTA(8HSYSDEBUG,70,ISYDBG,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HSYSDEBUG)
C
C   EXTRACT ARGUMENT IALL TO SEE WHETHER OR NOT TO TURN ON ALL
C   SYSTEM DEBUG
C
      IALL=0
      CALL HGTSTR(1,IARGS(63),ICKYES,IFL,ISTAT)
      IF(ISTAT.EQ.0)GO TO 110
      WRITE(IPR,602)ISTAT
      CALL ERROR
      GO TO 115
C
  110 IF(ICKYES.EQ.IYES)IALL=1
  115 NDEBGS=0
C
      IF(IALL.EQ.1)GO TO 125
C
C   LOOP THROUGH DEBUG CODES UNTIL A BLANK ONE IS FOUND
C
      DO 120 I=1,20
      CALL HGTSTR(1,IARGS(1+(I-1)*3),ICDS,IFL,ISTAT)
      IF(ISTAT.EQ.0)GO TO 118
      WRITE(IPR,602)ISTAT
      CALL ERROR
C
  118 IF(ICDS.EQ.IBLNK)GO TO 120
C
      NDEBGS=NDEBGS+1
      IDEBGS(NDEBGS)=ICDS
C
  120 CONTINUE
C
C     SET DEBUG FLAG - IBUG FOR PRINTING FC VARIABLES
C
  125 IBUG=IFBUG(ICBG)
C
C
      IF(IBUG.EQ.1)WRITE(IODBUG,908)IALL,NDEBGS
  908 FORMAT(1H0,5X,'*** IN EILUVO - IALL,NDEBGS= ',2I11)
      IF(IBUG.EQ.1.AND.NDEBGS.GT.0)WRITE(IODBUG,909)
     1     (IDEBGS(I),I=1,NDEBGS)
  909 FORMAT(1H0,5X,'SYSTEM DEBUG CODES CURRENTLY ACTIVE ARE'/
     1  6X,20(A4,1X))
C
C   SEE WHICH TECHNIQUE IS ON : ONESEG,FGROUP,CGROUP AND EXTRACT
C   RUNID FROM IT
C
      ITYPRN=0
      RUNID(1)=BLANK
      RUNID(2)=BLANK
C
      DO 10 I=1,3
      IX=4-I
      IF(I.EQ.1) CALL HPASTA(8HONESEG  ,70,IVAL,NWORDS,IARGS,ISTAT)
      IF(I.EQ.2) CALL HPASTA(8HFGROUP  ,70,IVAL,NWORDS,IARGS,ISTAT)
      IF(I.EQ.3) CALL HPASTA(8HCGROUP  ,70,IVAL,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,NAMRID(1,IX))
      CALL FTEKCK(IVALUE,NAMRID(1,IX),0,IVAL,0,1)
      IF(IVALUE.EQ.1)GO TO 15
   10 CONTINUE
C
      GO TO 20
C
   15 ITYPRN=IX
      CALL HGTSTR(2,IARGS,RUNID,IFL,ISTAT)
C
      IF(ISTAT.EQ.0)GO TO 20
      WRITE(IPR,602)ISTAT
  602 FORMAT(1H0,10X,'**ERROR** STATUS OF ',I3,' RETURNED FROM ',
     1  'HGTSTR')
      CALL ERROR
      DO 18 I=1,2
   18 RUNID(I)=BLANK
C
   20 IF(IBUG.EQ.1)WRITE(IODBUG,900)ITYPRN,RUNID
  900 FORMAT(1H0,5X,'*** IN EILUVO - ITYPRN= ',I3,', RUNID= ',2A4)
C
C
C   GET START DATE FROM TECHNIQUE STARTESP
C
      CALL HPASTA(8HSTARTESP,70,ISTESP,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HSTARTESP)
      IJDFC=IARGS(1)/24+1
      IHFC=IARGS(1)-IJDFC*24 + 24
C
      IF(IHFC.NE.24)GO TO 50
      IJDFC=IJDFC+1
      IHFC=0
C
   50 CONTINUE
   70 IF(IBUG.EQ.1)WRITE(IODBUG,901) IJDFC,IHFC
  901 FORMAT(1H0,5X,'*** IN EILUVO - IJDFC,IHFC ='/11X,2I11)
C
      CALL UDATL(IDATE)
      NOW(1)=IDATE(3)
      NOW(2)=IDATE(4)
      NOW(3)=IDATE(1)
      NOW(4)=IDATE(5)
      NOW(5)=IDATE(6)
C
C     SET INPTZC,  LOCAL AND NLSTZ FROM HCL RFC DEFAULT CB /HDFLTS/
C
      LOCAL=LCLHCL
      NLSTZ=LTZHCL
      INPTZC=LINPTZ
C
C   LOAD TECHNIQUES NOUTZ AND NOUTDS
C
      CALL HPAST(8HNOUTZ   ,NOUTZ,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HNOUTZ   )
      CALL FTEKCK(NOUTZ,8HNOUTZ   ,-6,NOUTZ,-12,12)
      CALL HPAST(8HNOUTDS  ,NOUTDS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HNOUTDS  )
      CALL FTEKCK(NOUTDS,8HNOUTDS  ,0,NOUTDS,0,1)
C
   22 IF(IBUG.EQ.1)WRITE(IODBUG,902)LOCAL,NOUTZ,NOUTDS,NLSTZ,INPTZC
  902 FORMAT(1H0,5X,'*** IN EILUVO - LOCAL,NOUTZ,NOUTDS,NLSTZ,INPTZC='/
     1  11X,4I11,5X,A4)
C
C   SET LDACPD AND LHRCPD
C
cew added call to get technique lstcmpdy for use in ssarr reg mods
cew no longer set them to 0
cew      LDACPD=0
cew      LHRCPD=0
C
cew set lstcmpdy to be = to startesp.  Only used in ssarr mods for
cew checking effective date.

         LDACPD=IJDFC
         LHRCPD=IHFC
        
      IF (IBUG.EQ.1) WRITE (IODBUG,440) IDARUN,IHRRUN,LDARUN,LHRRUN,
     *  LDACPD,LHRCPD
440   FORMAT ('0** IN EILUVO - IDARUN,IHRRUN,LDARUN,LHRRUN,',
     *  'LDACPD,LHRCPD='/11X,6I11)
C
C
C
C        SET NHOPDB AND NHOCAL FROM HCL RFC DEFAULT CB /HDFLTS/
C
      NHOPDB=NPDHCL
      NHOCAL=NCAHCL
      DO 100 I=1,8
  100 NHROFF(I)=0
C
      IF(IBUG.EQ.1)WRITE(IODBUG,905) NHOPDB,NHOCAL
  905 FORMAT(1H0,5X,'*** IN EILUVO - NHOPDB,NHOCAL='/
     1  11X,4I11)
C
C   SET SEARCH SWITCH FOR CALIBRATION FILES
C
      CALL HPAST(8HSEARCH  ,ISFN,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HSEARCH  )
      CALL FTEKCK(ISFN,8HSEARCH  ,0,ISFN,0,1)
C
C   LOAD ESP WINDOW INFORMATION FROM TECHNIQUE WINDOWS
C
      CALL HPASTA(8HWINDOWS ,70,NUMWIN,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HWINDOWS )
      CALL FTEKCK(NUMWIN,8HWINDOWS ,2,NUMWIN,1,5)
C
      DO 200 I=1,NUMWIN
      IPOS=1+14*(I-1)
      IWJD(I)=IARGS(IPOS)/24+1
      IWH(I)=IARGS(IPOS)-IWJD(I)*24+24
      IF(IWH(I).NE.0) GO TO 150
      IWJD(I)=IWJD(I)-1
      IWH(I)=IWH(I)+24
  150 IPOS=8+14*(I-1)
      LWJD(I)=IARGS(IPOS)/24+1
      LWH(I)=IARGS(IPOS)-LWJD(I)*24+24
      IF(LWH(I).NE.0) GO TO 200
      LWJD(I)=LWJD(I)-1
      LWH(I)=LWH(I)+24
  200 CONTINUE
C
C   LOAD HISTORICAL AND ADJUSTED SWITCHES
C
      CALL HPAST(8HHISTSIM ,JHSS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HHISTSIM )
      CALL FTEKCK(JHSS,8HHISTSIM ,0,JHSS,0,1)
C
      CALL HPAST(8HADJSIM  ,JASS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HADJSIM  )
      CALL FTEKCK(JASS,8HADJSIM  ,0,JASS,0,1)
C
C   LOAD WATER SUPPLY AND METRIC UNITS SWITCHES
C
      CALL HPAST(8HWSUNITS ,IWS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HWSUNITS )
      CALL FTEKCK(IWS,8HWSUNITS ,0,IWS,0,1)
C
      CALL HPAST(8HMETRIC  ,METRIC,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HMETRIC  )
      CALL FTEKCK(METRIC,8HMETRIC  ,0,METRIC,0,1)
C
C
C   LOAD THE INITIAL AND LAST HISTORICAL WATER YEARS
C
      CALL HPASTA(8HHISTWYRS,70,IY,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HHISTWYRS)
      CALL FTEKCK(IY,8HHISTWYRS,0,IY,0,1)
      IF(IY.GT.0) GO TO 300
      WRITE(IPR,605)
  605 FORMAT(1H0,10X,42H**ERROR** THE HISTORICAL WATER YEARS MUST ,
     1 11HBE DEFINED.)
C
      CALL ERROR
      CALL KILLFN(8HESP     )
      GO TO 999
  300 CONTINUE
      CALL FARGCK(IHYR,8HHISTWYRS,8HINITYEAR,0,IARGS(1),1900,2099)
      CALL FARGCK(LHYR,8HHISTWYRS,8HLASTYEAR,0,IARGS(2),1900,2099)
C
C   CHECK THAT LAST WATER YEAR IS GT FIRST
C
      IF(LHYR.GT.IHYR)GO TO 305
      LHYR=IHYR+1
      WRITE(IPR,642)IHYR,LHYR
  642 FORMAT(1H0,10X,'**WARNING** THE LAST WATER YEAR HAS BEEN ',
     1 'CHANGED SO THAT IT IS GREATER THAN THE FIRST WATER YEAR'/
     2 11X,'THE FIRST HISTORICAL DATA WATER YEAR IS ',I4/
     3 11X,'THE LAST  HISTORICAL DATA WATER YEAR IS ',I4)
      CALL WARN
C
C   LOAD BASE PERIOD INFORMATION
C
  305 CALL HPASTA(8HBASEPER ,70,JBPS,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HBASEPER )
      CALL FTEKCK(JBPS,8HBASEPER ,0,JBPS,0,1)
      IF(JBPS.NE.0) GO TO 350
      IBHYR=IHYR
      LBHYR=LHYR
      GO TO 355
  350 CALL FARGCK(IBHYR,8HBASEPER ,8HINITYEAR,0,IARGS(1),1900,2099)
      CALL FARGCK(LBHYR,8HBASEPER ,8HLASTYEAR,0,IARGS(2),1900,2099)

cew added numcosav technique here
C
C  NUMBER OF DAYS OF CARRYOVER TO BE SAVED IS VALUE OF TECH NUMCOSAV
C
 355  CALL HPASTA (8HNUMCOSAV,100,NCSTOR,NWORDS,IARGS,ISTAT)
      IF (ISTAT.GT.0) CALL FPHPWN (ISTAT,8HNUMCOSAV)
      CALL FTEKCK (NCSTOR,8HNUMCOSAV,0,NCSTOR,-1,10)
      IF (NCSTOR.LE.0) GO TO 360
C
C  DATES FOR SAVING CARRYOVER ARE IN ARGUMENTS OF TECH NUMCOSAV
C
      DO 356 I=1,NCSTOR
      ICDAY(I)=IARGS(1+(I-1)*7)/24+1
      ICHOUR(I)=IARGS(1+(I-1)*7)-ICDAY(I)*24+24
      IF (ICDAY(I).LE.1) GO TO 360
356   CONTINUE
c
c   GENTRACE technique - switch to read special carryover file
c   for generating historical traces.  One argument which is the
c   time zone code the file was written with.
c
C
360   call hpasta(8hGENTRACE,100,igen,nwords,iargs,istat)
      if(istat.ne.0) call fphpwn(istat,8hGENTRACE)
      call ftekck(igen,8hGENTRACE,0,igen,0,1)
      if(igen.eq.1) then
         call hgtstr(1,iargs,timezn,ifl,istat)
         if(istat.ne.0) then
            write(ipr,602) istat
            write(ipr,612)
  612       format(11x,'while decoding GENTRACE technique')
            call error
         endif
      endif
C
c
C   LOAD PERMANENT TIME SERIES UNIT NUMBERS
C
      CALL HPASTA(8HTSUNITS ,70,ITS,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HTSUNITS )
      CALL FARGCK(KEPERM(1),8HTSUNITS ,8HUNIT1   ,0,IARGS(1),0,99)
      CALL FARGCK(KEPERM(2),8HTSUNITS ,8HUNIT2   ,0,IARGS(2),0,99)
      CALL FARGCK(KEPERM(3),8HTSUNITS ,8HUNIT3   ,0,IARGS(3),0,99)
      CALL FARGCK(KEPERM(4),8HTSUNITS ,8HUNIT4   ,0,IARGS(4),0,99)
      CALL FARGCK(KEPERM(5),8HTSUNITS ,8HUNIT5   ,0,IARGS(5),0,99)
C
C
C   LOAD ESP HISTORICAL FILE UNIT NUMBERS
C
      CALL HPASTA(8HHISTUNIT,70,IUSESP,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HHISTUNIT)
      CALL FTEKCK(IUSESP,8HHISTUNIT,1,IUSESP,0,1)
      CALL FARGCK(NE1,8HHISTUNIT,8HESPFILE1,37,IARGS(1),37,50)
      CALL FARGCK(NE2,8HHISTUNIT,8HESPFILE2,50,IARGS(2),37,50)
C
C
C   LOAD YEAR WEIGHTS FOR THE HISTORICAL YEARS
C
      CALL HPASTA(8HYRWEIGHT,70,IYRWT,NWORDS,IARGS,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HYRWEIGHT)
      CALL FTEKCK(IYRWT,8HYRWEIGHT,0,IYRWT,0,1)
      CALL UMEMOV(IARGS(1),YRWT(1),50)

c

cew    technique to reset timing varaibles so that local standard time
cew    is set to internal time.  This is used to enable ingest of PQPF
cew    ensembles.

      CALL HPAST(8HPQPFTIME,IPQTM,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HPQPFTIME)
      CALL FTEKCK(IPQTM,8HPQPFTIME,0,IPQTM,0,1)
       if(ipqtm.eq.1) then
        nlstz=-12
        noutds=0
        local=0
        flocal=0

cew no histsim when using pqpf time
        jhss=0
       endif

cew
cew    technique to write PRSF to the header of the ESPTS files
cew    THE PRSF string is used as a switch in ESPADP

      CALL HPAST(8HPRSF    ,IPRSF,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HPRSF    )
      CALL FTEKCK(IPRSF,8HPRSF    ,0,IPRSF,0,1)

cew    initialize prsf variable
      prsf=''

      if (iprsf.eq.1) then
       prsf='PRSF'  
      endif

cew add techniques for filling mods common block.  For use with 
cew  ssarr mods.

C
C  TECHNIQUE MODUNITS - NO ARGUMENTS
C
      CALL HPAST (8HMODUNITS,IUMGEN,ISTAT)
      IF (ISTAT.GT.0) CALL FPHPWN (ISTAT,8HMODUNITS)
      CALL FTEKCK (IUMGEN,8HMODUNITS,2,IUMGEN,0,2)

C
C  SET DEFAULT MOD TIME ZONE CODE FROM ARGUMENT OF TECH MODTZC
C  IF MODTZC (MTZTEK) EQ ZERO OR ARG (MODTMP) EQ BLANKS USE INPTZC
C
      CALL HPASTA (8HMODTZC  ,100,MTZTEK,NWORDS,IARGS,ISTAT)
      IF (ISTAT.GT.0) CALL FPHPWN (ISTAT,8HMODTZC  )
      CALL FTEKCK (MTZTEK,8HMODTZC  ,0,MTZTEK,0,1)
C
      MODTZC=INPTZC
      IF (MTZTEK.EQ.0) GO TO 555
      CALL HGTSTR (1,IARGS,MODTMP,IFL,ISTAT)
C
      IF (ISTAT.EQ.0) GO TO 460
      WRITE (IPR,51) ISTAT
51    FORMAT ('0**ERROR** STATUS OF ',I3,' RETURNED FROM ',
     * 'HGTSTR')
      WRITE (IPR,450)
450   FORMAT (11X,'WHILE DECODING TIME ZONE CODE FOR TECHNIQUE MODTZC'/
     * 11X,'TIME ZONE CODE FROM FILE USERPARM IS ASSUMED.')
      CALL ERROR
      GO TO 999
C
460   IF (MODTMP.NE.IBLNK) MODTZC=MODTMP


CEW LOAD PREADJUST SWITCH
C
555   CALL HPAST(8HPREADJ  ,preadj,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HPREADJ  )
      CALL FTEKCK(preadj,8HPREADJ  ,0,preadj,0,1)
C
CMGM TECHNIQUE TO TURN ON OR OFF ADJUST-Q in ESP
C
      CALL HPAST(8HESPADJQ ,iespadjq,ISTAT)
      IF(ISTAT.NE.0) CALL FPHPWN(ISTAT,8HESPADJQ )
      CALL FTEKCK(iespadjq,8HESPADJQ ,0,iespadjq,0,1)
C
      
  999 CONTINUE
      OPNAME(1)=OPHOLD(1)
      OPNAME(2)=OPHOLD(2)
      IOPNUM=IOPHLD
C
      IF(ITRACE.GT.0)WRITE(IODBUG,601)
  601 FORMAT(1H0,5X,'*** EXIT EILUVO ***')
C
      RETURN
      END
