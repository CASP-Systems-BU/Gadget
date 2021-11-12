//
// Created by Showan Asyabi on 1/20/21.
//

#ifndef GADGET_VALUE_H
#define GADGET_VALUE_H

#include <memory>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <optional>

#include "include/gadget/distributions/distributionParamters.h"


class Value {
public:
    virtual ~Value() = default;
    enum ValueDistribs {
        ConstantDistrib = 0 , UniformRealDistrib, ExponentialDistrib,   NormalDistrib
    };

    Value() {
        /** TDDO Remove the big value- No need for it in new design
        bigValue = new char[1048576];
        for(int i = 0; i < 1048576; i++) {
            bigValue[i] = '*';
        }
         */
    }

    // TODO string???!!!
    /***
     *
     * @return  the next value according to the value distribution
     */
    //virtual std::string_view Next() = 0;
    virtual std::string Next() = 0;
    char*  bigValue;
};




class ConstantValue : public Value {
public:
    explicit ConstantValue(u_int64_t value) : value_(value) {
    }

    std::string Next() override {
         return std::string(value_,'0');
    }

private:
    uint64_t value_;

};


class UniformRealValue : public Value {
public:
    explicit UniformRealValue(double a, double b) : a_(a), b_(b) {
    }

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(a_, b_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return std::string(len,'0');
    }

private:
    double a_;
    double b_;
};


class ExponentialDistributionValue : public Value{
public:
    explicit ExponentialDistributionValue(double lambda) : lambda_(lambda) {}

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(lambda_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return std::string(len,'0');
    }

private:
    double  lambda_;
};


class NormalDistributionValue: public Value {
public:
    explicit NormalDistributionValue(double mean, double sd): mean_(mean), sd_(sd) {}

    std::string Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(mean_, sd_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return std::string(len,'0');
    }

private:
    double mean_;
    double sd_;
};


class ValueBuilder {
public:
    static std::optional<std::shared_ptr<Value>> BuildValue(uint16_t distType, std::shared_ptr<DistributionParameters>  params) {
        if (distType == Value::UniformRealDistrib ) {
            return std::make_shared<UniformRealValue>(params->a_real, params->b_real);
        } else if (distType == Value::ExponentialDistrib ) {
            return std::make_shared<ExponentialDistributionValue>(params->lambda);
        } else if (distType == Value::NormalDistrib) {
            return std::make_shared<NormalDistributionValue>(params->mean, params->sd);
        } if (distType == Value::ConstantDistrib) {
            return std::make_shared<ConstantValue>(params->constantInt);
        }

        return {}; // the distType is not valid
    }
};
#endif //GADGET_VALUE_H
