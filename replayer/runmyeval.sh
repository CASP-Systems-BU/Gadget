#!/bin/bash

# ycsb workload A--F

# memory usage:
# vectors in replayer: (8B+6B+8B)*1000000/1024/1024=25MB
# DB: 256MB
# flexible rom: 50MB


if [ "$#" -ne 1 ]; then
    echo "Wrong number of parameters"
    echo "1st arg - yes--with cgroup, no-without cgroup"
    exit 1
fi

cgroupsw=$1

#distributions=(zipfian uniform sequential hotspot latest)
#wlist=(a b c d f)
distributions=(zipfian uniform)
wlist=(a d f)

reqcount=1000        # num of distinct keys
oprcount=2000000        # num of operations

for dds in ${distributions[@]};
do
    #recordcount  operationcount  requestdistribution  readproportion  updateproportion  scanproportion  insertproportion  readmodifywriteproportion  name
  ./genycsbtrace.sh "$reqcount" "$oprcount" "$dds" 0.5   0.5   0   0     0    "$dds"_a
#  ./genycsbtrace.sh "$reqcount" "$oprcount" "$dds" 0.95  0.05  0   0     0    "$dds"_b
#  ./genycsbtrace.sh "$reqcount" "$oprcount" "$dds" 1     0     0   0     0    "$dds"_c
  ./genycsbtrace.sh "$reqcount" "$oprcount" "$dds" 0.95  0     0   0.05  0    "$dds"_d
  ./genycsbtrace.sh "$reqcount" "$oprcount" "$dds" 0.5   0     0   0     0.5  "$dds"_f
done

# --------------------------------------------------

rm tmp/* -rf
mkdir tmp
rm -f result.txt
touch result.txt
echo "workload  p99  avg  p50  ops  KBread  KBwrite  " >> result.txt

# --------------------------------------------------

for dds in ${distributions[@]};
do
  for wls in ${wlist[@]};
  do
    echo mywl_"$dds"_"$wls"

    ./y_breplayer workload/mywl_"$dds"_"$wls"_load workload/mywl_"$dds"_"$wls"_run mywl_"$wls"_"$dds"_b
    rm tmp/* -rf

    ./y_lreplayer workload/mywl_"$dds"_"$wls"_load workload/mywl_"$dds"_"$wls"_run mywl_"$wls"_"$dds"_l
    rm tmp/* -rf

    ./y_rreplayer workload/mywl_"$dds"_"$wls"_load workload/mywl_"$dds"_"$wls"_run mywl_"$wls"_"$dds"_r
    rm tmp/* -rf

    if [[ $wls = "f" ]]
    then
      #python3 rmwpair.py workload/mywl_"$dds"_f_run y
      ./y_freplayer workload/mywl_"$dds"_f_load workload/mywl_"$dds"_f_run mywl_f_"$dds"_f
    else
      ./y_freplayer workload/mywl_"$dds"_"$wls"_load workload/mywl_"$dds"_"$wls"_run mywl_"$wls"_"$dds"_f
    fi
    rm tmp/* -rf
  done
done


rm workload/mywl_*

ctt=$(date +%s)
mv result.txt result_"$cgroupsw".txt
