//
// Created by Showan Asyabi on 2/8/21.
//

#ifndef GADGET_KEYSIZE_H
#define GADGET_KEYSIZE_H
#include <optional>
#include "include/gadget/distributions/distributionParamters.h"

class KeySize {
public:
    virtual ~KeySize() = default;
    enum KeySizeDistribs {
        ConstantDistrib = 0 , UniformRealDistrib, ExponentialDistrib,   NormalDistrib
    };

    // TODO string???!!!
    /***
     *
     * @return  the next value according to the value distribution
     */
    virtual int64_t Next() = 0;
};




class ConstantKeySize : public KeySize{
public:
    explicit ConstantKeySize(u_int64_t value) : value_(value) {}

    int64_t Next() override {
        return value_;
    }

private:
    double  value_;

};


class UniformRealKeySize : public KeySize {
public:
    explicit UniformRealKeySize(double a, double b) : a_(a), b_(b) {}

    int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(a_, b_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double a_;
    double b_;
};


class ExponentialDistributionKeySize : public KeySize{
public:
    explicit ExponentialDistributionKeySize(double lambda) : lambda_(lambda) {}

    int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(lambda_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double  lambda_;
};


class NormalDistributionKeySize: public KeySize {
public:
    explicit NormalDistributionKeySize(double mean, double sd): mean_(mean), sd_(sd) {}

    int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(mean_, sd_);
        int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double mean_;
    double sd_;
};


class KeySizeBuilder {
public:
    /// TODO using v1 and v2 is confusing
    static std::optional<std::shared_ptr<KeySize>> BuildKeySize(uint16_t distributionType, std::shared_ptr<DistributionParameters> params) {
        if (distributionType == KeySize::UniformRealDistrib ) {
            return std::make_shared<UniformRealKeySize>(params->a_real, params->b_real);
        } else if (distributionType == KeySize::ExponentialDistrib ) {
            return std::make_shared<ExponentialDistributionKeySize>(params->lambda);
        } else if (distributionType == KeySize::NormalDistrib ) {
            return std::make_shared<NormalDistributionKeySize>(params->mean, params->sd);
        } if (distributionType == KeySize::ConstantDistrib) {
            return std::make_shared<ConstantKeySize>(params->constantInt);
        }
        return {}; // the distribution type is not valid
    }
};



#endif //GADGET_KEYSIZE_H
