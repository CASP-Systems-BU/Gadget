# Gadget: A New Benchmark Harness for Systematic and Robust Evaluation of Streaming State Stores

This repository contains instructions for reproducing the experiments in our EuroSys'22 paper "A New Benchmark Harness for Systematic and Robust Evaluation of Streaming State Stores".

Gadget is a benchmark for an accurate evaluation of KV stores for stateful streaming applications. Gadget supports one or more configurable data sources and simulates the internal operations of a stream processing system to generate realistic state access workloads. Gadget generates and issues state access requests to the KV store while collecting performance measurements on latency and throughput. Gadget currently supports four KV stores with different design and performance characteristics: RocksDB, Lethe, FASTER, and BerkeleyDB.


## Table of Contents
  - [Description & Requirements](#description--requirements)
  - [Reproducing the EuroSys22 results](#reproducing-the-eurosys22-results)
  - [Install and Run Gadget](#install-and-run-gadget)
  - [Configure Gadget](#configure-gadget)
  - [Experiments with Gadget](#experiments-with-gadget)
  - [Gadget Source Code](#gadget-source-code)
  - [Conference Paper](#conference-paper)
  - [License](#license)


## Project Structure

```bash
> tree .
├── configs                 # Instructions on configuring Gadget
├── experiments             # Instructions on  performing experiments with Gadget
├── replayer                # replay ycsb trace on KV stores
├── reproduceEuroSysResults # reproducing EuroSys22 paper results
├── src                     # Gadget Source Code
├── trces                   # Google borg traces
└── tests                   # Unit tests 
```

## Description & Requirements

**How to access**

Gadget can be downloaded from the following link: https://github.com/CASP-Systems-BU/Gadget

**Hardware dependencies**

We used the following hardware for our experiments:
We run all experiments on a dual-socket machine equipped with 12-core Intel Xeon 4116 CPU running at 2.1 GHz, 32 GB of RAM, and 512GB PC400 NVMe(SK hynix) hard disk. 

**Software dependencies**

We use Ubuntu 20.04 (Linux kernel version5.4). To evaluate a KV store performance using Gadget, you need to install the KV store on your system. To make this step straightforward, we provide a container that already has all KV stores discussed in this paper installed.

**Traces**

In our experiments, we use traces from Google's Borg system. These traces can be found here: Read more [here](./traces)

##  Reproducing the EuroSys22 results

**We have prepared CloudLab profile and a container image (with compiled Gadget and KV stores installed) for reviewers. Please see [here](./reproduceEuroSysResults) for a detailed description of reproducing the results presented in the EuroSys22 conference.**

## Install and Run Gadget

If you wish to compile Gadget and try it out on your system. You can follow the following steps:
1. install build essentials (e.g., compiler):
 'sudo sh install_build_essentals.sh'

2. compile Gadget:
```
    git clone git@github.com:CASP-Systems-BU/Gadget.git
    cd Gadget
    mkdir build 
    cd build
    cmake ..
    make
    cd src
```

3. To run gadget, you can use one of our config files provided in `configs` folder([here](./configs)).  
   
   Run Gadget by:
   
   `./gadget config.txt`. 
   
   This example generates the state access workload for a tumbling window operator. The results go to a file named `gadget.log`. Please note that to evaluate a KV store using Gadget, the KV store must be installed on the system. (please see [here](./replayer) for installing KV stores). 

 

## Configure Gadget
Please see [here](./configs) for a detailed description of the Gadget configuration.

## Experiments with Gadget
Please see [here](./experiments) for a detailed description of performing experiments with  Gadget.

## Gadget Source Code
Please see [here](./src) for a detailed description of the Gadget source code.

## Conference Paper

[**EuroSys**] A New Benchmark Harness for Systematic and Robust Evaluation of Streaming State Stores. Esmail Asyabi, Yuanli Wang, John Liagouris, Vasiliki Kalavri, Azer Bestavros. Proceedings of the 17th European Conference on Computer Systems, Rennes, France, April, 2022.

## License

Gadget is released under [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0.txt)


