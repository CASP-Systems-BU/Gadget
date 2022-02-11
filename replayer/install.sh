# replayer

## Install dependencies
apt update
apt-get install vim net-tools ssh openssh-server -y
apt-get install p7zip-full sysstat python3 python3-pip cmake unzip tar git gcc-8 g++-8 openjdk-11-jre-headless libgflags-dev libsnappy-dev zlib1g-dev libbz2-dev liblz4-dev libzstd-dev libaio-dev uuid-dev libtbb-dev libssl-dev libboost-all-dev -y
ln -s /usr/bin/g++-8 /usr/bin/g++
ln -s /usr/bin/gcc-8 /usr/bin/gcc
pip3 install matplotlib

## clean
rm -rf rocksdb6114
rm -rf ycsb-0.17.0-rocksdb
rm -rf FASTER
rm -rf lethe
rm -rf berkerlydb


#### YCSB
wget https://github.com/brianfrankcooper/YCSB/releases/download/0.17.0/ycsb-0.17.0.tar.gz
tar -zxvf ycsb-0.17.0.tar.gz
mv ycsb-0.17.0 ycsb-0.17.0-rocksdb


## Install kvstores
#### RocksDB
git clone https://github.com/facebook/rocksdb.git
mv rocksdb rocksdb6114
cd rocksdb6114
git checkout feb06e83b2fbe5fe0bb63e18f1d3bbdbdb1a73e0
cd ..

cd rocksdb6114
make clean
make static_lib -j12  EXTRA_CXXFLAGS=-fPIC EXTRA_CFLAGS=-fPIC USE_RTTI=1
cd ..


#### FASTER
git clone https://github.com/microsoft/FASTER.git
cd FASTER
git checkout 5501dfb24b079b2601665568bfbffcd54065bec1
cd ..
patch -p0 < faster_patch    # https://github.com/microsoft/FASTER/commit/4b45c91e85217531e4065f30ecc230b6523734f8#diff-4f1d923a282498709c41b2022f09ce6b6980c0f0e49b5e1917432bc701375df1

mkdir FASTER/cc/build
cd FASTER/cc/build
cmake cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../../..


#### Lethe
unzip vkalavri-rocksdb-fade-10592ede52ab.zip
mv vkalavri-rocksdb-fade-10592ede52ab lethe
# change delete_persistence_latency in lethe/examples/__working_branch/emu_environment.cc
sed -i 's/delete_persistence_latency = 2250/delete_persistence_latency = 10/' lethe/examples/__working_branch/emu_environment.cc

cd lethe
make static_lib -j12 EXTRA_CXXFLAGS=-fPIC EXTRA_CFLAGS=-fPIC USE_RTTI=1
cd ..


#### BerkeleyDB
# download from https://www.oracle.com/database/technologies/related/berkeleydb-downloads.html
# we are using berkeley-db-18.1.25
tar -zxvf berkeley-db-18.1.25.tar.gz
mv db-18.1.25/ berkerlydb
# fix an error in ./src/repmgr/repmgr_net.c (ref: https://stackoverflow.com/questions/51252922/berkleydb-doesnt-install-on-ubuntu/51391588)
patch -p0 < berkelydb_patch

cd berkerlydb
./dist/configure -enable-cxx -enable-stl
make -j12
make install
cd ..
