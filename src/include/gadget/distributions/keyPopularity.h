//
// Created by Showan Asyabi on 2/23/21.
//

#ifndef GADGET_KEYPOPULARITY_H
#define GADGET_KEYPOPULARITY_H
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <optional>




#include "include/gadget/distributions/distributionParamters.h"


class KeyPopularity {
public:
    virtual ~KeyPopularity() = default;
    enum KeyPopularityDistribs {
        ConstantDistrib = 0,   PoissonDistrib , GeometricDistrib, UniformIntDistrib,  TemporalIncreasing, ZipfDistrib, hotSpotDistrib, FileKeys
    };
    /**
     *  Make  a keySpaceSize unique keys u
     * @param keySpace
     */
    KeyPopularity (uint64_t keySpaceSize = 128) {
        keySpaceSize_ = keySpaceSize;
  // here i was trying to make key space with smallest possible space overhead. I changed  my mind later because it is slow for large
  // key space sizes. Now I am returning numbers as keys
        /*
        keySpaceSize_ = keySpaceSize;
        if (keySpaceSize_ < 128) {
            keySpaceSize_ = 128;
        }
        // we assume each byte can represent 128 characters not 256
        // num_bytes that are needed to make  keySpaceSize unique keys - log a base b = log a / log b
        int64_t num_bytes = std::ceil(std::log(keySpaceSize_) / std::log (128));
        std::vector<char> key;
        key.resize(num_bytes, 0);

        int64_t index;
        uint16_t carry;
        int64_t key_cnt = 0;
        do {
            key_cnt ++;
            index = num_bytes - 1;
            carry = 1;
            keySpace.push_back(key);
            while (carry > 0 && index >= 0) {
                key[index]  = (key[index]+ 1) % 128;
                if  (key[index] == 0) {
                    carry = 1;
                    index --;
                } else {
                    carry = 0;
                }
            }

        } while ( index >= 0 && key_cnt <  keySpaceSize_);
         */
    }

    /***
     *
     * @return   then next key according the to the key distribution
     */
    virtual std::string Next() = 0;

    /**
     * This function returns the probability o seeing the last generated key
     * @return
     */
    virtual double ProbabilityLastKey() = 0;
    std::vector<std::vector<char>>  keySpace;
    uint64_t  keySpaceSize_;

};

class  ConstantKeyPopularity: public  KeyPopularity {
public:
    explicit ConstantKeyPopularity(const std::string& key): key_(key) {}

    std::string Next() override {
        return key_;
    }

  double  ProbabilityLastKey() override {
      return  1;
    }
private:
    std::string  key_;
};

class  TemporalIncreasingKeyPopularity: public  KeyPopularity {
public:
    explicit TemporalIncreasingKeyPopularity(uint64_t keySpaceSize): KeyPopularity(keySpaceSize), key_(0) {}

    /*
    std::string Next() override {
        std::string keyStr = std::string(keySpace[key_].begin(), keySpace[key_].end());
        key_ = (key_ + 1) % keySpaceSize_;
        return keyStr;
    }
    */
    std::string Next() override {
        key_ = key_ + 1;
        return std::to_string(key_);
    }

    double  ProbabilityLastKey() override {
        return  1; // fixme
    }

private:
    u_int64_t key_;
};

class PoissonDistributionKeyPopularity : public KeyPopularity {
public:
    explicit PoissonDistributionKeyPopularity(double  lambda, uint64_t keySpaceSize) : KeyPopularity(keySpaceSize), lambda_(lambda) {}

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::poisson_distribution<> distrib( lambda_);
        int64_t value = round(distrib(gen));
        value =  value % keySpaceSize_;
        //return std::string(keySpace[value].begin(), keySpace[value].end());
        return std::to_string(value);
    }

    double  ProbabilityLastKey() override {
        return  1; // fixme
    }

private:
    double  lambda_;
};


class FileKeys: public KeyPopularity {
public:
    explicit  FileKeys(std::string pathToFile) {
        std::ifstream fileStream(pathToFile);
        std::string line;
        std::vector<std::string> row;
        uint64_t  counter = 0;

        while (std::getline(fileStream, line)) {
            counter ++;
            if(counter == 1) { // skip the fist line
                continue;
            }
            std::istringstream is_line(line);
            std::string field;
            while (std::getline(is_line, field, ',')) {
                row.push_back(field);
            }
            keys_.push_back(row[row.size() - 1]);
            // print the firts 100 keys
            if(keys_.size() < 100 ) {
                std::cout << "key: "<< keys_.back() << std::endl;
            }
        }
        keySpaceSize_ = keys_.size();
        fileStream.close();
        std::cout << " file reading is done" << std::endl;
    }

    std::string Next() override {
        currentKey_++;
        uint64_t index = currentKey_ % keySpaceSize_;
        return keys_[index];
    }

    double  ProbabilityLastKey() override {
        return  1; // fixme
    }

private:
std::vector<std::string>  keys_;
uint64_t  currentKey_ =0;
};

class ECDF : public KeyPopularity {
public:
    explicit ECDF(const std::string& pathToECDF): c_(0) {

        std::ifstream fileStream(pathToECDF);
        std::string line;
        std::vector<std::string> row;
        double  probability;

        while (std::getline(fileStream, line)) {
            std::istringstream is_line(line);
            std::string field;
            while (std::getline(is_line, field,',')) {
                row.push_back(field);
            }
            std::cout<< line <<std::endl;
            probability = std::stod(row[row.size() -1]);
            probabilities.push_back(probability);
        }
        keySpaceSize_ = probabilities.size();
        fileStream.close();
        std::cout << " file reading is done" <<std::endl;

    }

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(0, 1);
        double zValue = 0;
        while (zValue == 0) {
            zValue = distrib(gen);
        }

        uint64_t  min = 1, max = keySpaceSize_, mid;
        do {
            mid =std::floor((min+max) / 2);
            if (probabilities[mid] >= zValue && probabilities[mid - 1] < zValue) {
                lastKeyIndex_ = mid;
                return std::to_string(mid);
            } else if(  probabilities[mid] >= zValue ) {
                max = mid - 1;
            } else {
                min = mid + 1;
            }
        } while ( min <= max);
        lastKeyIndex_ = keySpaceSize_;
        return std::to_string(keySpaceSize_);  // fixme
    }



    double  ProbabilityLastKey() override { // fixme remove these pe
        return  probabilities[keySpaceSize_] - probabilities[lastKeyIndex_ -  1];
    }

private:
    double  c_;
    uint64_t keySpaceSize_;
    std::vector<double> probabilities;
    uint64_t lastKeyIndex_;
};


class ZipfDistributionKeyPopularity : public KeyPopularity {
public:
    explicit ZipfDistributionKeyPopularity(double  s = 1, uint64_t keySpaceSize = 1000000): c_(0), keySpaceSize_(keySpaceSize) {
        probabilities.resize(keySpaceSize + 1 , 0);

        // compute normalization constant
        for (int i = 1; i <= keySpaceSize; i++) {
            c_ = c_ + (1.0 / pow((double) i, s));
        }
        c_ = 1 / c_;

        // compute the probabilities
        double sumProbability = 0;
        for(int i = 1; i <= keySpaceSize; i++) {
            sumProbability += (c_/ pow(i,s));
            probabilities[i] = sumProbability;
        }

    };

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(0, 1);
        double zValue = 0;
        while (zValue == 0) {
            zValue = distrib(gen);
        }
        // binary search
        uint64_t  min = 1, max = keySpaceSize_, mid;
        do {
            mid =std::floor((min+max) / 2);
            if (probabilities[mid] >= zValue && probabilities[mid - 1] < zValue) {
                lastKeyIndex_ = mid;
                return std::to_string(mid);
            } else if(  probabilities[mid] >= zValue ) {
                max = mid - 1;
            } else {
                min = mid + 1;
            }
        } while ( min <= max);
        lastKeyIndex_ = keySpaceSize_;
        return std::to_string(keySpaceSize_);  // fixme
    }

    double  ProbabilityLastKey() override {
        return  probabilities[keySpaceSize_] - probabilities[lastKeyIndex_ -  1];
    }

private:
    double  c_;
    uint64_t keySpaceSize_;
    std::vector<double> probabilities;
    uint64_t lastKeyIndex_;
};

class GeometricDistributionKeyPopularity : public KeyPopularity {
public:
    explicit GeometricDistributionKeyPopularity(float p , uint64_t keySpaceSize) : KeyPopularity(keySpaceSize), p_(p) {}

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::geometric_distribution<> distrib( p_);
        int64_t value = round(distrib(gen));
        value =  value % keySpaceSize_;
        //return  std::string(keySpace[value].begin(), keySpace[value].end());
        return std::to_string(value);
    }

    double  ProbabilityLastKey() override {
        // fixme
        return 1;
    }

private:
    double  p_;
};

/**
 * This class is for hot spot  keys where a portion of operations(operationFraction_)
 * use  a small portion (hotSpotFraction_) of key space
 */
class HotSpotKeyPopularity : public KeyPopularity {
public:
    explicit HotSpotKeyPopularity(double hotSpotFraction, double hotOperationFraction, uint64_t keySpaceSize) :hotSpotFraction_(hotSpotFraction), hotOperationFraction_(hotOperationFraction), keySpaceSize_(keySpaceSize) {
        hot_interval_ = round(hotSpotFraction_ * keySpaceSize_);
        isChosenKeyHot_ = false;
    }

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(0, 1);
        float_t value = distrib(gen);
        if(value <= hotOperationFraction_) { // if it is a hot operation
            value = distrib(gen);
            isChosenKeyHot_ = true;
            return std::to_string(std::floor(value * hot_interval_));
        } else {
            value = distrib(gen);
            isChosenKeyHot_ = false;
            return std::to_string( hot_interval_ + std::floor(value * (keySpaceSize_ - hot_interval_)));
        }
    }

    double  ProbabilityLastKey() override {
        if( isChosenKeyHot_) {
            hotOperationFraction_  * (1/ hot_interval_);
        } else {
            (1 - hotOperationFraction_)  * (1/ (keySpaceSize_ - hot_interval_) );
        }
    }

private:
    double hotSpotFraction_;
    double hotOperationFraction_;
    int64_t keySpaceSize_;
    int64_t hot_interval_;
    bool isChosenKeyHot_;

};


class UniformIntImpKeyPopularity : public KeyPopularity{
public:
    explicit UniformIntImpKeyPopularity(int64_t a, int64_t b , uint64_t keySpaceSize) :KeyPopularity(keySpaceSize), a_(a), b_(b) {
        keySpaceSize_ = keySpaceSize;
    }

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_int_distribution<> distrib(a_, b_);
        int64_t value = round(distrib(gen));
        value =  value % keySpaceSize_;
        return std::to_string(value);
    }
    double  ProbabilityLastKey() override {
        1 / (b_ - a_);
    }

private:
    int64_t a_;
    int64_t b_;
};

class  KeyPopularityBuilder {
public:
    static std::optional<std::shared_ptr<KeyPopularity>> BuildKeyPopularity(uint16_t type, std::shared_ptr<DistributionParameters> parameters, uint64_t keySpaceSize) {
        if (type == KeyPopularity::PoissonDistrib ) {
            return std::make_shared<PoissonDistributionKeyPopularity>(parameters->lambda, keySpaceSize );
        } else if (type == KeyPopularity::GeometricDistrib ) {
            return std::make_shared<GeometricDistributionKeyPopularity>(parameters->p , keySpaceSize);
        } else if (type == KeyPopularity::UniformIntDistrib ) {
            return std::make_shared<UniformIntImpKeyPopularity>(parameters->a_int, parameters->b_int , keySpaceSize);
        } else if (type == KeyPopularity::TemporalIncreasing ) {
            return std::make_shared<TemporalIncreasingKeyPopularity>(keySpaceSize);
        } else if (type == KeyPopularity::ConstantDistrib ) {
            return std::make_shared<ConstantKeyPopularity>(parameters->constantString);
        } else if (type == KeyPopularity::ZipfDistrib ) {
            return std::make_shared<ZipfDistributionKeyPopularity>(parameters->s, keySpaceSize);
        } else if (type == KeyPopularity::hotSpotDistrib ) {
            return std::make_shared<HotSpotKeyPopularity>(parameters->hotSpotFraction, parameters->hotOperationFraction, keySpaceSize);
        } else if (type == KeyPopularity::FileKeys ) {
            return std::make_shared<FileKeys>(parameters->pathToFile);
        }
        return {};
    }
};

#endif //GADGET_KEYPOPULARITY_H
