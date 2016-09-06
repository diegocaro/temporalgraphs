TYPEGRAPH=$1
BITT=$2
BITB=$3
BITC=$4
INPUT=$5
OUTPUT=$6

echo BitmapT $BITT
echo BitmapB $BITB
echo BitmapC $BITC
echo Typegraph $TYPEGRAPH
echo Reading $INPUT

HEADER=`gzcat $INPUT|head -n1`
LEVELS=`python -c "import math; h = '$HEADER'.split(); h=[int(x) for x in h]; print int(math.ceil(math.log(max(h[0],h[2]))/math.log(2)));"`
echo Levels in $INPUT: $LEVELS

#for ((lf=4; lf <= $LEVELS; lf+=4 ))
#do
#    OUTFILE=$OUTPUT-2,2,0,0,$lf,0-$BITT-$BITB.mxf
#	if [ ! -f "$OUTFILE" ]; then
#    echo Creating $OUTFILE
#    gzcat $INPUT | ./create -s MXF -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$lki,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
#	fi
#done

#for ((l=0; l <= $LEVELS; l+=4 ))
#do
#    lki=$((l*2))
#    
##    OUTFILE=$OUTPUT-2,2,0,$lki,0,0-$BITT-$BITB.mxd
##	if [ ! -f "$OUTFILE" ]; then
##		echo Creating $OUTFILE
##		gzcat $INPUT | ./create -s MXD -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$lki,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
##	fi
#	
#    OUTFILE=$OUTPUT-2,2,0,$lki,0,0-$BITT-$BITB.prb
#    if [ ! -f "$OUTFILE" ]; then
#		echo Creating $OUTFILE
#		gzcat $INPUT | ./create -s PRB -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$lki,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
#	fi	
##    OUTFILE=$OUTPUT-2,2,0,$lki,0,0-$BITT-$BITB.prw
##	if [ ! -f "$OUTFILE" ]; then
##    echo Creating $OUTFILE
##    gzcat $INPUT | ./create -s PRW -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$lki,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
##	fi
#done

OUTFILE=$OUTPUT-2,2,0,0,0,0-$BITT-$BITB.prb
echo Creating $OUTFILE
if [ ! -f "$OUTFILE" ]; then
    gzcat $INPUT | ./create -s PRB -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:0,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
fi 

OUTFILE=$OUTPUT-2,2,0,$(( LEVELS*2 )),0,0-$BITT-$BITB.prb
   echo Creating $OUTFILE
if [ ! -f "$OUTFILE" ]; then
   gzcat $INPUT | ./create -s PRB -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$(( LEVELS*2 )),lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
fi

OUTFILE=$OUTPUT-2,2,0,$LEVELS,0,0-$BITT-$BITB.prb
    echo Creating $OUTFILE
if [ ! -f "$OUTFILE" ]; then
    gzcat $INPUT | ./create -s PRB -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$LEVELS,lf:0,F:0,T:$BITT,B:$BITB,C:$BITC $OUTFILE
fi 







for F in 2 8 16 64
do
    OUTFILE=$OUTPUT-2,2,0,0,0,$F-$BITT-$BITB-$BITC.prb2
    	echo Creating $OUTFILE

	if [ ! -f "$OUTFILE" ]; then
		gzcat $INPUT | ./create -s PRB2 -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:0,lf:0,F:$F,T:$BITT,B:$BITB,C:$BITC $OUTFILE
	fi
	
    OUTFILE=$OUTPUT-2,2,0,$(( LEVELS*2 )),0,$F-$BITT-$BITB-$BITC.prb2
    	echo Creating $OUTFILE

	if [ ! -f "$OUTFILE" ]; then
		gzcat $INPUT | ./create -s PRB2 -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$(( LEVELS*2 )),lf:0,F:$F,T:$BITT,B:$BITB,C:$BITC $OUTFILE
	fi
	
    OUTFILE=$OUTPUT-2,2,0,$LEVELS,0,$F-$BITT-$BITB-$BITC.prb2
    	echo Creating $OUTFILE

	if [ ! -f "$OUTFILE" ]; then
		gzcat $INPUT | ./create -s PRB2 -g $TYPEGRAPH -f k1:2,k2:2,lk1:0,lki:$LEVELS,lf:0,F:$F,T:$BITT,B:$BITB,C:$BITC $OUTFILE
	fi
done
