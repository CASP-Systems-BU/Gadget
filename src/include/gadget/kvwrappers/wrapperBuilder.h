//
// Created by Showan Asyabi on 1/22/21.
//

#ifndef GADGET_WRAPPERBUILDER_H
#define GADGET_WRAPPERBUILDER_H
#include "include/gadget/kvwrappers/kvwrapper.h"
#include "include/gadget/kvwrappers/RocksDBWrapper.h"
#include "include/gadget/kvwrappers/fileWrapper.h"
#include "include/gadget/kvwrappers/lethe.h"
#include "include/gadget/kvwrappers/berecklyDB.h"
#include "include/gadget/kvwrappers/faster.h"

#include <memory>
#include <string>
#include <vector>
#include <optional>

class WrapperBuilder {
public:
    static std::optional<std::shared_ptr<KVWrapper>> BuildWrapper(KVWrapper::KVType kvType, std::string path) {
        if(kvType == KVWrapper::RocksDB) {
            return std::make_shared<RocksDBWrapper>(path);
        }  if(kvType == KVWrapper::Faster) {
            return std::make_shared<FasterWrapper>(path);
        } if(kvType == KVWrapper::Lethe) {
            return std::make_shared<LetheWrapper>(path);
        } if(kvType == KVWrapper::BerkeleyDB) {
            return std::make_shared<BerkeleyDBWrapper>(path);
        } else if(kvType == KVWrapper::File) {
            return std::make_shared<FileWrapper>(path);
        }
        return  {};
    }

};
#endif //GADGET_WRAPPERBUILDER_H
