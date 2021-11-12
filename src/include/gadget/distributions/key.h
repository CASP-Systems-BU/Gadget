//
// Created by Showan Asyabi on 1/20/21.
//

#ifndef GADGET_KEY_H
#define GADGET_KEY_H
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <random>
#include <optional>


#include "keyPopularity.h"
#include "keySize.h"
#include "distributionParamters.h"



class Key {
public:
    virtual ~Key() = default;
 enum KeyDistribs {
     ConstantDistrib = 0,   PoissonDistrib , GeometricDistrib, UniformIntDistrib,  TemporalIncreasing
 };

 Key (std::shared_ptr<KeyPopularity> keyPopularity, std::shared_ptr<KeySize> keySize ):keyPopularity_(keyPopularity), keySize_(keySize) {}

    /***
     * make the key with the desired popularity and length
     * @return  the next using  key popularity  distribution and key length distribution
     */
    std::string Next() {
        std::string key = keyPopularity_->Next();
        uint64_t  key_size = keySize_->Next();
        while (key.length() < key_size) {
            // add  0 to the beginning  of the key if th e key size is less than desired
            key =  '0' + key;
        }
        return key;

    }

private:
    std::shared_ptr<KeyPopularity> keyPopularity_;
    std::shared_ptr<KeySize> keySize_;

};



class  KeyBuilder {
public:
   static  std::shared_ptr<Key> BuildKey(std::shared_ptr<KeyPopularity> KeyPopularityDist, std::shared_ptr<KeySize> KeySizeDist) {
            return std::make_shared<Key>(KeyPopularityDist, KeySizeDist);
    }
};


#endif //GADGET_KEY_H
