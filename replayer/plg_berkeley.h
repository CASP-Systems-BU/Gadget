//
// Created by fl on 3/22/21.
//

#ifndef REPLAYER_PLG_BERKELEY_H
#define REPLAYER_PLG_BERKELEY_H

#include "plg.h"

#include <db_cxx.h>


class plg_berkeley {
public:
    int opendb();
    int closedb();

    double opr_insert(uint64_t ikey, const char* val);
    double opr_update(uint64_t ikey, const char* val);
    double opr_read(uint64_t ikey);
    double opr_delete(uint64_t ikey);
    double opr_merge(uint64_t ikey, const char* val);

    Db* pdb;
    //DbEnv env;

};


#endif //REPLAYER_PLG_BERKELEY_H
