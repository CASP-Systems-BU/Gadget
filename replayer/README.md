# replayer

replay ycsb traces on KV stores

## Run replayer

- remove previous generated ycsb trace. `rm workload/mywl_*`

- run experiments
```
./compile.sh
./runfig12.sh
```

The figures will be generated here, with filename `fig12_plot_*`

----------------------------------------------------

you can config the size of YCSB traces according to the free space of your hard disk, by changing `reqcount` in `runeval.sh`

you may need to change the directory of `JAVA_HOME` in `genycsbtrace.sh`
 
