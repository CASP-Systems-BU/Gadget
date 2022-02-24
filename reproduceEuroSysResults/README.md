
Gadget results presented in Eurossys 22 conference can be reproduced in two ways:
- Cloudlab
- Your local machine


# Table of Contents
  - [Reproducing Results on Cloudlab](#description--requirements)
  - [Reproducing Results on your machine]()
  - [Major Claims](#major-claims)
  - [Experiments and expected results](#experiments-and-expected-results)



# Reproducing Results on Cloudlab

On Cloudlab, instantiate the following public profile: 
https://www.cloudlab.us/p/easyabi/gadget50

This profile gets an m510 machine from the Utah cluster, downloads docker, and installs all requirements.
Please note that m510 does not match our hardware, but it is close enough to see the result trends and paper claims.
Once inside the machine, please perform the following commands to download the gadget container from the docker hub and run the container:
```
cd /local
sudo sh init.sh
```
Next, to start the experiments, please do the following commands:
```
cd /home/gadget/build/src/
./runAllExprs.sh
```

This command does all the experiments. Once done( it takes around 8 hours), the results will be in folders: firstExpr/ secondExpr/ thirdExpr/

Please also see some other commands that you might find useful:

To exit from the container: `exit`

To run the container : `sudo docker run -t -i gadget7200/gadget7200:1 /bin/bash`

To ssh to the container (password is 1): `ssh root@172.17.0.2`


# Reproducing Results on your machine (more detailed instructions)

To make this step straightforward, we provide a container that already has all KV stores discussed in this paper installed. Here we present  the instructions to produce the results presented in Eurosys paper in more detail. 


## Downloads the Gadget container  and run it

To evaluate Gadget, one should take the following steps:

- Download the Gadget container from the following link: https://drive.google.com/drive/folders/198RUZYpE0Sfb1Fn1wGeJDrK7M1N3eRQr

- Run the container using the following commands:

  - Install docker on your computer

  - Import the downloaded image. `sudo docker import gadget_cont.tar`

  - Use `sudo docker images` List all images on the machine, and remember the ID of the image added to the system. For example: 482b416487f3

  - Start the container with this image(The root password for the container is 1): `sudo docker run -t -i 482b416487f3 /bin/bash`

  - Go to the gadget folder and run the experiments by running shell scripts designed for each experiment. `cd /home/gadget/build/src/`

- You may also get the container from the docker hub and run it:
```
sudo docker pull gadget7200/gadget007:1
sudo docker run -t -i  gadget7200/gadget007:1  /bin/bash
```


# Major Claims

- (C1): In section 6.1, we show that Gadget faithfully simulates streaming state accesses and can produce workloads that exhibit the characteristics of real traces. In Figure 10, we analyze the generated traces `/home/gadget/build/src/gadgett/` and compare them to the real ones `/home/gadget/build/src/flinkt/` in terms of temporal and spatial locality. According to Figure 10,  Gadget produces a trace that consists of an almost identical number of unique sequences as the real trace. The distribution of stack distances in Gadget traces is also very close to that of real traces.

- (C2): In section 6.2, we evaluate the KV store performance using YCSB workloads located in `/home/gadget/build/src/ycsbt`, real Flink workloads located in `/home/gadget/build/src/flinkt/`, and Gadget workloads. We observe that the performance (Throughput and latency) achieved with Gadget workloads is very close to that measured using the real traces for all operators and KV stores. On the contrary, when using the tuned YCSB workloads, the reported throughput and latency are significantly varied from the performance measured using the real traces.

- (C3): Finally, in section 6.3, we evaluate the performance of KV stores for all operators using Gadget. Figure 13 plots throughput and tail latency for all kv stores. We see that RocksDB is significantly outperformed in six out of eleven workloads by both FASTER and BerkeleyDB.

- (C4): In Section 6.3, we also evaluate the performance of existing KV stores using YCSB. Figure 12 plots throughput and tail latency for all KV stores.

# Experiments and expected results

## Experiment (E1):

**[Claim 1] [1 human-hour]**

This experiment runs Gadget to generate traces for three operators and compares it with real traces (located in `/home/gadget/build/src/flinkt/`) in terms of spatial and temporal locality.

- run the container as described before

- go to gadget folder `cd /home/gadget/build/src/`

- run the experiment script `./firstExpr.sh`

- see the result in folder experiment1. `cd firstExpr`


**Results**

The results will be located in folder `firstExpr`. The generated figures should be similar to those shown in Figure 10.


## Experiment (E2):

**[Claim 2] [3 human-hours]**

This experiment evaluates kv stores using gadget generated traces, YCSB traces (located in `/home/gadget/build/src/ycsbt`) and real traces (located in `/home/gadget/build/src/flinkt/`) in terms of throughput and latency.

- run the container as described before

- go to gadget folder `cd /home/gadget/build/src/`

- run the experiment script `./secondExpr.sh`

- see the result in folder experiment2 `cd secondExpr`

**Results**

The results will be located in folder `secondExpr`. The generated figures should be similar to those shown in Figure 11.


## Experiment (E3):

**[Claim 3] [3 human-hours]**

This experiment compares the performance of KV stores for all operators using Gadget.

- run the container as described before

- go to gadget folder `cd /home/gadget/build/src/`

- run the experiment script `./thirdExpr.sh `

- see the result in folder experiment3 `cd thirdExpr`

**Results**

The results will be located in folder `thirdExpr`. The generated figures should be similar to those shown in Figure 13

## Experiment (E4):

**[Claim 4] [1 human-hour]**

- run the container as described before

- go to replayer folder `cd /home/replayer/`
-  `./compile.sh`
- remove previous generated ycsb trace if any: `rm workload/mywl_*`

- run experiments:  `python3 runfig12.py`

- The figures will be generated  with filenames `fig12_plot_*`

**Results**

The generated figures should be similar to those shown in Figure 12.

**Please note that the hardware differences  may change  the value of some bars in performance figures.**
