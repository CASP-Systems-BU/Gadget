#!/bin/bash

set -ex


if [ "$#" -ne 9 ]; then
    echo "Wrong number of parameters"
    echo "1st arg - recordcount"
    echo "2nd arg - operationcount"
    echo "3rd arg - requestdistribution"
    echo "4th arg - readproportion"
    echo "5th arg - updateproportion"
    echo "6th arg - scanproportion"
    echo "7th arg - insertproportion"
    echo "8th arg - readmodifywriteproportion"
    echo "9th arg - name"
    exit 1
fi

reqcount=$1                     #_RECCOUNT_
oprcount=$2                     #_OPRCOUNT_
distribution=$3                 #_REQDISTRIB_
readpercent=$4                  #_REDPERCENT_
updatepercent=$5                #_UPDPERCENT_
scanpercent=$6                  #_SCNPERCENT_
insertpercent=$7                #_INSPERCENT_
rmwpercent=$8                   #_RMWPERCENT_
wlname=$9

cp ./workload/mywltemplate ./workload/mywl_"$wlname"

sed -i "s#_RECCOUNT_#$reqcount#g" ./workload/mywl_"$wlname"
sed -i "s#_OPRCOUNT_#$oprcount#g" ./workload/mywl_"$wlname"
sed -i "s#_REQDISTRIB_#$distribution#g" ./workload/mywl_"$wlname"
sed -i "s#_REDPERCENT_#$readpercent#g" ./workload/mywl_"$wlname"
sed -i "s#_UPDPERCENT_#$updatepercent#g" ./workload/mywl_"$wlname"
sed -i "s#_SCNPERCENT_#$scanpercent#g" ./workload/mywl_"$wlname"
sed -i "s#_INSPERCENT_#$insertpercent#g" ./workload/mywl_"$wlname"
sed -i "s#_RMWPERCENT_#$rmwpercent#g" ./workload/mywl_"$wlname"

export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64/

./ycsb-0.17.0-rocksdb/bin/ycsb.sh load basic -s -P ./workload/mywl_"$wlname" >> ./workload/mywl_"$wlname"_load
./ycsb-0.17.0-rocksdb/bin/ycsb.sh run basic -s -P ./workload/mywl_"$wlname" >> ./workload/mywl_"$wlname"_run
