//
// Created by Showan Asyabi on 1/20/21.
//

#ifndef GADGET_ARRIVAL_H
#define GADGET_ARRIVAL_H


#include <memory>
#include <string>
#include <vector>
#include <random>
#include <optional>
#include "include/gadget/distributions/distributionParamters.h"


class Arrival {
public:
    virtual ~Arrival() = default;

    enum ArrivalDistribs {
        ConstantDistrib = 0 , UniformRealDistrib, ExponentialDistrib,   NormalDistrib
    };
    /**
     * Return  the waiting time based the  distribution of the inter arrivals
     * @param rate
     * @return
     */
    virtual double  Next() = 0;

/**
 * This function is for distributions that parameters of the distributions change over time
 * @param params
 * @return
 */
    virtual double  Next(std::shared_ptr<DistributionParameters> params) = 0;
};


class ConstantArrival : public Arrival{
public:
    explicit ConstantArrival(uint64_t value) : value_(value) {}

     double Next() override {
        return value_;
    }

    double Next(std::shared_ptr<DistributionParameters> params) override {
        return params->constantInt;
    }

private:
    uint64_t value_;

};


class UniformRealArrival : public Arrival {
public:
    explicit UniformRealArrival(double a, double b) : a_(a), b_(b) {}

     double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(a_, b_);
        return distrib(gen) ;
    }

    double Next(std::shared_ptr<DistributionParameters> params) override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(params->a_real, params->b_real);
        return distrib(gen) ;
    }

private:
    double a_;
    double b_;
};


class ExponentialDistributionArrival : public Arrival{
public:
    explicit ExponentialDistributionArrival(double lambda) : lambda_(lambda) {}

     double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(lambda_);
        return distrib(gen) ;
    }

    double Next(std::shared_ptr<DistributionParameters> params) override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(params->lambda);
        return distrib(gen) ;
    }

private:
    double  lambda_;
};


class NormalDistributionArrival : public Arrival {
public:
    explicit NormalDistributionArrival(double mean, double sd): mean_(mean), sd_(sd) {}

    double Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(mean_, sd_);
        return distrib(gen) ;
    }

    double Next(std::shared_ptr<DistributionParameters> params) override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(params->mean, params->sd);
        return distrib(gen) ;
    }

private:
    double mean_;
    double sd_;
};

class ArrivalBuilder {
public:
    static std::optional<std::shared_ptr<Arrival>> BuildArrival(uint16_t distType, std::shared_ptr<DistributionParameters>  params) {
        if (distType == Arrival::UniformRealDistrib ) {
            return std::make_shared<UniformRealArrival>(params->a_real, params->b_real);
        } else if (distType == Arrival::ExponentialDistrib ) {
            return std::make_shared<ExponentialDistributionArrival>(params->lambda);
        } else if (distType == Arrival::NormalDistrib) {
            return std::make_shared<NormalDistributionArrival>(params->mean, params->sd);
        } else if (distType == Arrival::ConstantDistrib) {
            return std::make_shared<ConstantArrival>(params->constantInt);
        }

        return {}; // the distType is not valid
    }
};


#endif //GADGET_ARRIVAL_H
