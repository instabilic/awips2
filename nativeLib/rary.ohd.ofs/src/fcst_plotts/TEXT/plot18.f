C MEMBER PLOT18
C  (from old member FCEX18)
C-----------------------------------------------------------------------
C
C                             LAST UPDATE: 03/22/95.10:10:21 BY $WC20SV
C
C @PROCESS LVL(77)
C
      SUBROUTINE PLOT18 (PO,D,ILOCD,J1,J2,LZERO,LTENS,NTENS,ORD,LOCPT,
     1 TORD,NLOCD)
C
C   THIS ROUTINE IS CALLED BY THE EXECUTION ROUTINE
C   OF THE PLOT-TS OPERATION. IT PLOTS THE DATA FOR THE
C   PERIOD J1 TO J2 .
C
C   THIS ROUTINE WAS INITIALLY PROGRAMMED BY
C   GERALD N. DAY - HRL  MAY 1980
C
C   ARGUMENTS -
C
C     PO - P ARRAY (INPUT)
C     D - D ARRAY (INPUT)
C     ILOCD - FIRST LOCATION IN THE D ARRAY FOR EACH TIME
C             SERIES (INPUT)
C     J1 - FIRST JULIAN DAY TO PLOT (INPUT)
C     J2 - LAST JULIAN DAY TO PLOT (INPUT)
C     LZERO - PLOT POSITIONS OF THE X AXIS FOR EACH PLOT (INPUT)
C     LTENS - PLOT POSITIONS TO MARK THE Y AXIS (INPUT)
C     NTENS - THE TOTAL NUMBER OF PLOT POSITIONS USED TO
C             MARK THE Y AXIS. (INPUT)
C     ORD - PLOT ARRAY (WORK)
C     LOCPT - PLOT POSITIONS THAT ARE CHANGED IN THE ORD ARRAY (WORK)
C     TORD - CHARACTERS THAT ARE CHANGED IN THE ORD ARRAY (WORK)
C     NLOCD - NEXT LOCATION IN THE D ARRAY FOR EACH TIME SERIES (WORK)
C
      DIMENSION D(1),ILOCD(1),PO(*),LZERO(1),LTENS(12),ORD(121),
     1 LOCPT(1),TORD(1),NLOCD(1),NDAYS(24),AMONTH(12)
      COMMON/IONUM/IN,IPR,IPU
      COMMON/FENGMT/METRIC
      COMMON/FCTIME/IDARUN,IHRRUN,LDARUN,LHRRUN,LDACPD,
     1 LHRCPD,NOW(5),LOCAL,NOUTZ,NOUTDS,NLSTZ,IDA,IHR,
     2 LDA,LHR,IDADAT
      COMMON/FDBUG/IODBUG,ITRACE,IDBALL,NDEBUG,IDEBUG(20)
C
C    ================================= RCS keyword statements ==========
      CHARACTER*68     RCSKW1,RCSKW2
      DATA             RCSKW1,RCSKW2 /                                 '
     .$Source: /fs/hseb/ob72/rfc/ofs/src/fcst_plotts/RCS/plot18.f,v $
     . $',                                                             '
     .$Id: plot18.f,v 1.1 1995/09/17 18:58:30 dws Exp $
     . $' /
C    ===================================================================
C
      DATA BLANK/1H /,DOT/1H./,SYMI/1HI/,PTYPE/4HARIT/
      DATA NDAYS/31,31,28,29,31,31,30,30,31,31,30,30,31,31,31,31,
     1 30,30,31,31,30,30,31,31/
      DATA AMONTH/4HJAN ,4HFEB ,4HMAR ,4HAPR ,4HMAY ,4HJUN ,4HJUL ,
     1 4HAUG ,4HSEP ,4HOCT ,4HNOV ,4HDEC /
C
      NPLOTS=PO(8)
      NTTS=PO(9)
      IDTP=PO(11)
C
C   CALCULATE THE MAIN LOOP INDICES
C
      K1=(J1-IDADAT)*24/IDTP+IHR/IDTP
      K2=(J2-IDADAT)*24/IDTP+LHR/IDTP
C
C   INITIALIZE THE PLOT ARRAY
C
      DO 10 I=1,121
      ORD(I)=BLANK
   10 CONTINUE
      DO 20 I=1,NTENS
      LOCDOT=LTENS(I)
      ORD(LOCDOT)=DOT
   20 CONTINUE
      DO 30 I=1,NPLOTS
      LOCI=LZERO(I)
      ORD(LOCI)=SYMI
   30 CONTINUE
      CALL MDYH1(J1,IHR,IMOTZ,IDATZ,IYRTZ,IHRTZ,NOUTZ,NOUTDS,TZONE)
C
C   CHECK FOR LEAP YEAR
C
      LY=0
      IF(IYRTZ.EQ.(4*(IYRTZ/4))) LY=1
      IF(IYRTZ.NE.(100*(IYRTZ/100))) GO TO 40
      IF(IYRTZ.EQ.(400*(IYRTZ/400))) LY=0
   40 CONTINUE
      DO 100 I=1,NTTS
      IPO=11+9*NPLOTS+12*(I-1)
      IDT=PO(IPO+4)
      NVPDT=PO(IPO+11)
      LOCV=PO(IPO+12)
C
C   CALCULATE THE LOCATION OF THE FIRST VALUE IN THE D ARRAY TO
C   BE PLOTTED FOR EACH TIME SERIES.
C
      NLOCD(I)=((J1-IDADAT)*24/IDT)*NVPDT+((IHR-1)/IDT)*NVPDT+LOCV+
     1 ILOCD(I)-1
  100 CONTINUE
      IHR1=IHR
      DO 500 K=K1,K2
      IF(K.EQ.K1) GO TO 110
C
C   INCREMENT TIME
C
      IHR1=IHR1+IDTP
      IF(IHR1.LE.24) GO TO 105
      IHR1=IHR1-24
  105 IHRTZ=IHRTZ+IDTP
      IF(IHRTZ.LE.24) GO TO 110
      IHRTZ=IHRTZ-24
      IDATZ=IDATZ+1
      MOIND=2*IMOTZ+LY-1
      KDAYS=NDAYS(MOIND)
      IF(IDATZ.LE.KDAYS) GO TO 110
      IDATZ=1
      IMOTZ=IMOTZ+1
      IF(IMOTZ.LE.12) GO TO 108
      IMOTZ=1
      IYRTZ=IYRTZ+1
      LY=0
      IF(IYRTZ.EQ.(4*(IYRTZ/4))) LY=1
      IF(IYRTZ.NE.(100*(IYRTZ/100))) GO TO 108
      IF(IYRTZ.EQ.(400*(IYRTZ/400))) LY=0
  108 WRITE(IPR,820) AMONTH(IMOTZ)
  110 CONTINUE
      NTSKNT=0
      NTSPLT=0
      DO 400 I=1,NPLOTS
      IPO=11+9*(I-1)
      TYPE=PO(IPO+1)
      NPTS=PO(IPO+2)
      PMIN=PO(IPO+3)
      PMAX=PO(IPO+4)
      NTS=PO(IPO+5)
      IF(METRIC.EQ.1) GO TO 112
C
C   CONVERT MAX/MIN ORDINATES TO ENGLISH UNITS
C
      CFACT=PO(IPO+8)
      CONST=PO(IPO+9)
      PMIN=PMIN*CFACT+CONST
      PMAX=PMAX*CFACT+CONST
  112 IF(TYPE.EQ.PTYPE) GO TO 115
C
C   CONVERT MAX/MIN ORDINATES TO LOGS FOR LOG PLOTS
C
      PMIN=ALOG10(PMIN)
      PMAX=ALOG10(PMAX)
  115 PTS=NPTS
      DELTA=(PMAX-PMIN)/PTS
      DO 300 J=1,NTS
      IPO=11+9*NPLOTS+12*(NTSKNT+J-1)
      IDT=PO(IPO+4)
      SYMBOL=PO(IPO+8)
      NVPDT=PO(IPO+11)
C
C   CHECK IF CURRENT TIME IS A MULTIPLE OF THE TIME SERIES TIME
C   INTERVAL.
      IF(IHR1.NE.(IDT*(IHR1/IDT))) GO TO 300
      II=NLOCD(NTSKNT+J)
      VALUE=D(II)
C
C   UPDATE THE LOCATION OF THE NEXT DATA VALUE IN THE D ARRAY.
C
      NLOCD(NTSKNT+J)=II+NVPDT
      IF(METRIC.EQ.1) GO TO 116
C
C   CONVERT DATA VALUE TO ENGLISH UNITS
C
      VALUE=VALUE*CFACT+CONST
  116 IF(TYPE.EQ.PTYPE) GO TO 120
C
C   CONVERT DATA VALUE TO LOG FOR LOG PLOTS.
C
      IF(VALUE.LE.0.0) GO TO 118
      VALUE=ALOG10(VALUE)
      GO TO 120
  118 VALUE=PMIN
C
C   CALCULATE THE PLOTTING POSITION
C
  120 JJ=LZERO(I)+(VALUE-PMIN)/DELTA
      IF(VALUE.LE.PMIN) JJ=LZERO(I)
      IF(VALUE.GE.PMAX) JJ=LZERO(I)+NPTS-1
C
      IF(ORD(JJ).NE.BLANK.AND.ORD(JJ).NE.DOT.AND.ORD(JJ).NE.SYMI)
     1 GO TO 130
C
C   STORE THE BACKGROUND PLOT POSITION AND SYMBOL
C
      NTSPLT=NTSPLT+1
      LOCPT(NTSPLT)=JJ
      TORD(NTSPLT)=ORD(JJ)
C   STORE THE TS PLOT SYMBOL
C
  130 ORD(JJ)=SYMBOL
  300 CONTINUE
      NTSKNT=NTSKNT+NTS
  400 CONTINUE
C
C
C   PLOT
C
      IF(IDTP.EQ.24) GO TO 410
      WRITE(IPR,800) IDATZ,IHRTZ,(ORD(JJ),JJ=1,121)
      GO TO 420
  410 WRITE(IPR,810) IDATZ,(ORD(JJ),JJ=1,121)
  420 CONTINUE
C
C   RESET THE ORD ARRAY.
C
      IF(NTSPLT.EQ.0) GO TO 500
C
      DO 450 J=1,NTSPLT
      JJ=LOCPT(J)
      ORD(JJ)=TORD(J)
  450 CONTINUE
  500 CONTINUE
      IF(ITRACE.GE.1) WRITE(IODBUG,910)
      RETURN
  800 FORMAT(2X,I2,2X,I2,3X,121A1)
  810 FORMAT(5X,I2,4X,121A1)
  820 FORMAT(1H0,A4)
  900 FORMAT(1H0,17H** PLOT18 ENTERED)
  910 FORMAT(1H0,16H** PLOT18 EXITED)
      END
