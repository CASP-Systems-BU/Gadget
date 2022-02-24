# Gadget in action

Gadget can be used to generate state store workload for operators it currently supports. In addition, Gadget users can evaluate the performance of a KV store for an operator. To this end, users need to install the KV store and modify the Gadget CMake file with the path of the installed KV store. Here, we present the CMake files and instructions to evaluate the performance of RocksDB, BerkeleyDB, and Faster.


## Experiment with RocksDB
- Install RocksDB on your system
- Do the following commands to copy the RocksDB wrapper:
```
cd src/include/gadget/kvwrappers/
cp rocksdb.main RocksDBWrapper.h 
cd -
```
- Now copy the content of CMake.Rocks to the Gadget's main CMake file
```
cp experiments/CMake.Rocks src/CMakeLists.txt 
```
- Change lines 65 of 'src/CMakeLists.txt' to the path of your installed RocksDB
- Now compile Gadget and user a config file to do the experiment. In the config file, set the wrapper type of rocksdb


## Experiment with Faster
- Install Faster on your system
- Do the following commands to copy the Faster wrapper:
```
cd src/include/gadget/kvwrappers/
cp faster.main faster.h 
cd -
```
- Now copy the content of CMake.Faster to the Gadget's main CMake file
```
cp experiments/CMake.Faster  src/CMakeLists.txt 
```
- Change lines 65 of 'src/CMakeLists.txt' to the path of your installed Faster
- Now compile Gadget and user a config file to do the experiment. In the config file, set the wrapper type of `faster`

## Experiment with BerkeleyDB
- Install BerkeleyDB on your system
- Do the following commands to copy the BerkeleyDB wrapper:
```
cd src/include/gadget/kvwrappers/
cp berkeley.main berecklyDB.h
cd -
```
- Now copy the content of CMake.BerkeleyDB to the Gadget's main CMake file
```
cp experiments/CMake.BerkeleyDB  src/CMakeLists.txt 
```
- Change lines 65 of 'src/CMakeLists.txt' to the path of your installed BerkeleyDB
- Now compile Gadget and user a config file to do the experiment. In the config file, set the wrapper type of `berkeley
