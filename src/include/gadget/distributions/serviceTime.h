//
// Created by Showan Asyabi on 3/4/21.
//

#ifndef GADGET_SERVICETIME_H
#define GADGET_SERVICETIME_H

#include <memory>
#include <string>
#include <vector>
#include <random>
#include <optional>
#include <iostream>

#include "include/gadget/distributions/distributionParamters.h"

class ServiceTime {
public:
    virtual ~ServiceTime() = default;

    enum ServiceTimeDistribs {
        ConstantDistrib = 0 , UniformRealDistrib, ExponentialDistrib,   NormalDistrib
    };
    /**
     * Return  the waiting time based the  distribution of the inter arrivals
     * @param rate
     * @return
     */
    virtual double  Next() = 0;
};


class ConstantServiceTime : public ServiceTime{
public:
    explicit ConstantServiceTime(double value) : value_(value) {}

    double Next() override {
        return value_;
    }

private:
    double  value_;

};


class UniformRealServiceTime : public ServiceTime {
public:
    explicit UniformRealServiceTime(double a, double b) : a_(a), b_(b) {}

    double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(a_, b_);
        return distrib(gen) ;
    }

private:
    double  a_;
    double b_;
};


class ExponentialDistributionServiceTime : public ServiceTime{
public:
    explicit ExponentialDistributionServiceTime(double lambda) : lambda_(lambda) {}

    double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(lambda_);
        double retValue = distrib(gen);
        return retValue;
    }

private:
    double  lambda_;
};


class NormalDistributionServiceTime : public ServiceTime {
public:
    explicit NormalDistributionServiceTime(double mean, double sd): mean_(mean), sd_(sd) {}

    double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(mean_, sd_);
        return distrib(gen) ;
    }

private:
    double mean_;
    double sd_;
};

class ServiceTimeBuilder {
public:
    static std::optional<std::shared_ptr<ServiceTime>> BuildServiceTime(uint16_t distType, std::shared_ptr<DistributionParameters>  params) {
        if (distType == ServiceTime::UniformRealDistrib ) {
            return std::make_shared<UniformRealServiceTime>(params->a_real, params->b_real);
        } else if (distType == ServiceTime::ExponentialDistrib ) {
            return std::make_shared<ExponentialDistributionServiceTime>(params->lambda);
        } else if (distType == ServiceTime::NormalDistrib) {
            return std::make_shared<NormalDistributionServiceTime>(params->mean, params->sd);
        }  if (distType == ServiceTime::ConstantDistrib) {
            return std::make_shared<ConstantServiceTime>(params->constantReal);
        }

        return {}; // the distType is not valid
    }
};


#endif //GADGET_SERVICETIME_H
