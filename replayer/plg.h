//
// Created by fl on 12/24/20.
//

#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <assert.h>
#include <string.h>
#include <chrono>
#include <atomic>
#include <cinttypes>
#include <numeric>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>

#include <boost/algorithm/string/find.hpp>
#include <functional> //for std::hash

#ifndef REPLAYER_PLG_H
#define REPLAYER_PLG_H

#define VALUESIZE 256

class plg {

    virtual int opendb()=0;
    virtual int closedb()=0;

    virtual double opr_insert(uint64_t ikey, const char* val)=0;
    virtual double opr_update(uint64_t ikey, const char* val)=0;
    virtual double opr_read(uint64_t ikey)=0;
    virtual double opr_delete(uint64_t ikey)=0;
    virtual double opr_merge(uint64_t ikey, const char* val)=0;
};


#endif //REPLAYER_PLG_H
