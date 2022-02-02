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
