rm -f rreplayer lreplayer freplayer breplayer

# FASTER
echo "Compile FASTER: "
g++ replayer.cpp plg_faster.cpp plg.cpp -o y_freplayer -IFASTER/cc/src/ ./FASTER/cc/build/libfaster.a  -lstdc++fs -luuid -ltbb -lgcc -laio -lm -lstdc++ -lpthread -DRUNF

# RocksDB
echo "Compile RocksDB: "
g++ replayer.cpp plg_rocksdb.cpp plg.cpp -o y_rreplayer -Irocksdb6114/include ./rocksdb6114/librocksdb.a -lpthread -lsnappy -lz -lbz2 -lzstd -ldl -llz4 -fno-rtti -DRUNR

# lethe
echo "Compile lethe: "
g++ replayer.cpp plg_rocksdb.cpp plg.cpp -o y_lreplayer lethe/examples/__working_branch/stats.cc lethe/examples/__working_branch/emu_environment.cc -Ilethe/include -Ilethe/examples/__working_branch/ ./lethe/librocksdb.a -lpthread -lsnappy -lz -lbz2 -lzstd -ldl -llz4 -fno-rtti -DRUNR

# BerkeleyDB
echo "Compile BerkeleyDB: "
g++ replayer.cpp plg_berkeley.cpp plg.cpp -o y_breplayer ./berkerlydb/libdb_cxx.a -Iberkerlydb -lpthread -lssl -lcrypto -DRUNB

