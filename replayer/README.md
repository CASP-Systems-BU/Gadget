# replayer

replay ycsb traces on KV stores


# Table of Contents

- [Install dependencies](#install-dependencies)
- [Install kvstores](#install-kvstores)
  - [RocksDB](#rocksdb)
  - [FASTER](#faster)
  - [Lethe](#lethe)
  - [BerkeleyDB](#berkeleydb)
- [Run replayer](#run-replayer)




## Install dependencies

```
sudo apt update
sudo apt-get install vim net-tools ssh openssh-server -y
sudo apt-get install p7zip-full sysstat python3 python3-pip cmake unzip tar git gcc-8 g++-8 openjdk-11-jre-headless libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev libaio-dev uuid-dev libtbb-dev libssl-dev libboost-all-dev -y
sudo ln -s /usr/bin/g++-8 /usr/bin/g++
sudo ln -s /usr/bin/gcc-8 /usr/bin/gcc
sudo pip3 install matplotlib

```

#### YCSB

- Install

```
wget https://github.com/brianfrankcooper/YCSB/releases/download/0.17.0/ycsb-0.17.0.tar.gz
tar -zxvf ycsb-0.17.0.tar.gz
mv ycsb-0.17.0 ycsb-0.17.0-rocksdb

```


#### clean previously installed KV srotes

- Make sure you did not install them before

```
## clean
rm -rf rocksdb6114
rm -rf ycsb-0.17.0-rocksdb
rm -rf FASTER
rm -rf lethe
rm -rf berkerlydb
```

## Install kvstores

#### RocksDB


- Download 
```
git clone https://github.com/facebook/rocksdb.git
mv rocksdb rocksdb6114
cd rocksdb6114
git checkout feb06e83b2fbe5fe0bb63e18f1d3bbdbdb1a73e0
cd ..

```

- Install 

```
cd rocksdb6114
make clean
make static_lib -j12  EXTRA_CXXFLAGS=-fPIC EXTRA_CFLAGS=-fPIC USE_RTTI=1
cd ..

```


#### FASTER

- Download 
```
git clone https://github.com/microsoft/FASTER.git
cd FASTER
git checkout 5501dfb24b079b2601665568bfbffcd54065bec1
cd ..
patch -p0 < faster_patch    # fix a bug in faster: https://github.com/microsoft/FASTER/commit/4b45c91e85217531e4065f30ecc230b6523734f8#diff-4f1d923a282498709c41b2022f09ce6b6980c0f0e49b5e1917432bc701375df1

```

- Install 

```
mkdir FASTER/cc/build 
cd FASTER/cc/build
cmake cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../../..

```

#### Lethe

- Download. User need to manually download `vkalavri-rocksdb-fade-10592ede52ab.zip` and place it in `./replayer/`. Since lethe is not open-sourced yet, you can [contact the author](http://disc-projects.bu.edu/lethe/) to get this file.

```
unzip vkalavri-rocksdb-fade-10592ede52ab.zip
mv vkalavri-rocksdb-fade-10592ede52ab lethe

#change delete_persistence_latency in lethe/examples/__working_branch/emu_environment.cc
sed -i 's/delete_persistence_latency = 2250/delete_persistence_latency = 10/' lethe/examples/__working_branch/emu_environment.cc
```

- Install

```
cd lethe
make static_lib -j12  EXTRA_CXXFLAGS=-fPIC EXTRA_CFLAGS=-fPIC USE_RTTI=1
cd ..

```

#### BerkeleyDB


- Download. User need to manually download `berkeley-db-18.1.25` from https://www.oracle.com/database/technologies/related/berkeleydb-downloads.html , and place it in `./replayer/`.

```
# download from https://www.oracle.com/database/technologies/related/berkeleydb-downloads.html    
# we are using berkeley-db-18.1.25
tar -zxvf berkeley-db-18.1.25.tar.gz
mv db-18.1.25/ berkerlydb

# fix an error in ./src/repmgr/repmgr_net.c (ref: https://stackoverflow.com/questions/51252922/berkleydb-doesnt-install-on-ubuntu/51391588)
patch -p0 < berkelydb_patch

```

- Install

```
cd berkerlydb
./dist/configure -enable-cxx -enable-stl
make -j12
sudo make install
cd ..

```



## Run replayer

- remove previous generated ycsb trace. `rm workload/mywl_*`

- change the path of KV stores in  `./compile.sh`.

- run experiments
```
./compile.sh
./runfig12.sh
```

The figures will be generated here, with filename `fig12_plot_*`

----------------------------------------------------

you can config the size of YCSB traces according to the free space of your hard disk, by changing `reqcount` in `runeval.sh`

you may need to change the directory of `JAVA_HOME` in `genycsbtrace.sh`
 
