# Gadget: A New Benchmark Harness for Systematic and Robust Evaluation of Streaming State Stores

Gadget is a benchmark for an accurate evaluation of kv stores for stateful streaming applications.  Gadget supports one or more configurable data sources and simulates the internal operations of a stream processing system to generate realistic state access workloads.  Gadget generates and issues state access requests to the kv store while collecting performance measurements on latency and throughput.  Gadget  currently supports four kv stores with different design and performance characteristics: RocksDB, Lethe, FASTER , and BerkeleyDB.

# Description & Requirements

## How to access

Gadget can be downloaded from the following link: https://github.com/CASP-Systems-BU/Gadget

## Hardware dependencies

We used the following hardware for our experiments:
We run all experiments on a dual-socket machine equipped with 12-core Intel Xeon 4116 CPU running at 2.1 GHz, 32 GB of RAM, and 512GB PC400 NVMe(SK hynix) hard disk. 

## Software dependencies

We  use  Ubuntu 20.04 (Linux  kernel version5.4). To evaluate an KV store performance using Gadget, you need to install the KV store on you system.  To make this step straightforward, we provide a container that already has all KV stores discussed in this paper installed.

## Benchmarks

In our experiments, we use traces from Google's Borg system. The provided container includes these traces.

# Set-up

## Evaluation workflow

To evaluate Gadget, one should take the following steps:

- Download the  Gadget container from the following link: https://drive.google.com/drive/folders/198RUZYpE0Sfb1Fn1wGeJDrK7M1N3eRQr

- Run the container using the following commands:

-- Install docker on your computer 

-- Import the downloaded image. `sudo docker import gadget_cont.tar`

-- Use `sudo docker images` List all images on the machine, and remember the ID of the image added to the system.  For example: 482b416487f3

-- Start the container with this image(The root password for the container is 1): `sudo docker run -t -i 482b416487f3  /bin/bash`

-- Go the gadget folder and run the experiments by running shell scripts designed for each experiment. `cd /home/gadget/build/src/`

If you wish to compile Gadget, and try it out on your system. You can follow the following steps:

```
    Downlaod gadget 
    cd Gadget
    mkdir build 
    cd build
    cmake ..
    make
    cd src
```

To run gadget, you can use one of our config files provided in `configs` folder  and run Gadget by `./gadget config.txt`
 
 
This example  reads Borg task traces and generates the state access workload for  a tumbling  window operator. The results go to a file named `gadget.log`. Note that to evaluate a KV store using Gadget, the KV store must be installed on the system.


# Major Claims

- (C1): In section 6.1, we show that Gadget faithfully simulates streaming state accesses and can produce workloads that exhibit the characteristics of real traces. In Figure 10, we analyze the generated traces and compare them to the real ones in terms of temporal and spatial locality. According to Figure 10,  Gadget produces a trace that consists of an almost identical number of unique sequences as the real trace. The distribution of stack distances in Gadget traces is also very close to that of real traces.

- (C2): In section 6.2,  we evaluate the KV store performance using  YCSB workloads located in `/home/gadget/build/src/ycsbt`, real Flink workloads located in `/home/gadget/build/src/flinkt/`, and Gadget workloads. We observe that the performance  (Throughput and latency) achieved with Gadget workloads is very close to that measured using the real traces for all operators and kv stores. On the contrary, when using the tuned YCSB workloads, the reported throughput and latency are significantly varied from the performance measured using the real traces. 
    
- (C3): Finally, in section 6.3,  we evaluate the performance of KV stores for all operators using Gadget. Figure 13 plots throughput and tail latency for all kv stores. We see that RocksDB is significantly outperformed in six out of eleven workloads by both FASTER and BerkeleyDB. 

- (C4): In Section 6.3, we also evaluate the performance of KV stores using YCSB. Figure 12 plots throughput and tail latency for all kv stores.

## Experiments

### Experiment (E1): [Claim 1] [1 human-hour]: 

This experiment  runs Gadget to generate traces for three operators and compares it with real traces (located in `/home/gadget/build/src/flinkt/`) in terms of spatial and temporal locality.

- run the container as described before
    
- go to gadget folder `cd /home/gadget/build/src/`
    
- run the experiment script `./firstExpr.sh`
    
- see the result in folder experiment1. `cd firstExpr`
 

#### [Results]

The results will be located in folder `firstExpr`. The generated figures should be similar to those shown in Figure 10.


### Experiment (E2): [Claim 2] [3 human-hours] 

This experiment evaluates kv stores using gadget generated traces, YCSB traces (located in  `/home/gadget/build/src/ycsbt`) and real traces (located in `/home/gadget/build/src/flinkt/`) in terms  of throughput and latency.  

- run the container as described before
    
- go to gadget folder `cd /home/gadget/build/src/`
    
- run the experiment script `./secondExpr.sh`

- see the result in folder experiment2   `cd secondExpr` 

#### [Results]

The results will be located in folder `secondExpr`. The generated figures should be similar to those shown in Figure 11.


### Experiment (E3): [Claim 3] [3 human-hours]

This experiment compares  the performance of  KV stores for all operators using Gadget.

- run the container as described before

- go to gadget folder `cd /home/gadget/build/src/`

- run the experiment script `./thirdExpr.sh ` 

- see the result in folder experiment3 `cd thirdExpr`

#### [Results]

The results will be located in folder `thirdExpr`. The generated figures should be similar to those shown in Figure 13

### Experiment (E4): [Claim 4] [1 human-hour]

- run the container as described before

- go to `cd /home/replayer/`

- run the experiment script `./runmyeval.sh no`. Repeat it for 3 times so it will generate 3 results.

- see the name of the result files. `ls /home/replayer/` . Remember these names (eg: `result_no_1633719187.txt` , `result_no_1633719795.txt` , `result_no_1633720380.txt` )

- run the script to generate figure. 

```
python3 plot2.py result_no_1633719187.txt result_no_1633719795.txt result_no_1633720380.txt ops
python3 plot2.py result_no_1633719187.txt result_no_1633719795.txt result_no_1633720380.txt p99
```

- see the results `result_plot__*` in `/home/replayer/`. 

#### [Results]

The generated figures should be similar to those shown in Figure 12
