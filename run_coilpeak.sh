#/bin/sh

RUN_DIR=`pwd`
CS_FILE=`ls Det_*Ms_xy.dat`
TS_FILE=`ls Det_*Ts1_xy.dat`

# check if the echo has option -e or not
if test "`echo -e`" = "-e" ; then ECHO=echo; else ECHO="echo -e"; fi

$ECHO
$ECHO "--------------------------------------------------------------"
$ECHO " Running the postprocessing"
$ECHO "--------------------------------------------------------------"

# environment
SCRIPT_DIR=$WORK/magrad/script
SCRIPT_FILE="coildeposit.py"
PLOT_DIR=$WORK/magrad/radloss
PLOT_FILE="FindPeak.exe"

# check the scripts
for DIR in "$SCRIPT_DIR" "$PLOT_DIR" ; do
    if test ! -d $DIR ; then
        $ECHO
        $ECHO "Error: $DIR did not exist."
        $ECHO "Aborting"
        exit 1
    fi
done

for FILE in "$SCRIPT_DIR/$SCRIPT_FILE" "$PLOT_DIR/$PLOT_FILE" ; do
    if test ! -x $FILE ; then
        $ECHO
        $ECHO "Error: $FILE did not exist."
        $ECHO "Aborting"
        exit 1
    fi
done

# match file
$ECHO "file lists:"
for FILE in $CS_FILE $TS_FILE ; do
    if test ! -f $FILE ; then
        $ECHO "Aborting"
        exit 1
    fi
    echo -e "  - $FILE"
done
$ECHO

# clean root file
rm -rf *.root

# converting to root file
for FILE in $CS_FILE $TS_FILE ; do
    $SCRIPT_DIR/$SCRIPT_FILE $FILE
    $ECHO
done

$ECHO "done"
$ECHO
