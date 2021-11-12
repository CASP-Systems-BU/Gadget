//
// Created by Showan Asyabi on 2/1/21.
//

#ifndef GADGET_WINDOWLENGTH_H
#define GADGET_WINDOWLENGTH_H

#include <optional>
#include "include/gadget/distributions/distributionParamters.h"

class WindowLength {
public:
    virtual ~WindowLength() = default;
    enum WindowLengthDistribs {
        ConstantDistrib = 0 , UniformRealDistrib, ExponentialDistrib,   NormalDistrib
    };

    // TODO string???!!!
    /***
     *
     * @return  the next value according to the value distribution
     */
    virtual u_int64_t Next() = 0;
};




class ConstantWindowLength : public WindowLength{
public:
    explicit ConstantWindowLength(u_int64_t value) : value_(value) {}

    u_int64_t Next() override {
        return value_;
    }

private:
    u_int64_t  value_;

};



class UniformRealWindowLength : public WindowLength {
public:
    explicit UniformRealWindowLength(double a, double b) : a_(a), b_(b) {}

    u_int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(a_, b_);
        u_int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double a_;
    double b_;
};


class ECDFWindowLength : public WindowLength {
public:
    explicit ECDFWindowLength(const std::string& pathToECDF): c_(0) {

        std::ifstream fileStream(pathToECDF);
        std::string line;
        std::vector<std::string> row;
        double  probability;
        uint64_t  xValue;
        uint64_t  lineCounter;

        while (std::getline(fileStream, line)) {
            lineCounter ++;
            if  (lineCounter < 2) {
                continue; // do not read the first line -  header of csv
            }
            std::istringstream is_line(line);
            std::string field;
            while (std::getline(is_line, field,',')) {
                row.push_back(field);
            }
            std::cout<< line <<std::endl;
            try {
                xValue = std::stoll(row[row.size() - 2]);
                probability = std::stod(row[row.size() - 1]);
            } catch (...) {
                std::cout << "error in reading the file" << std::endl;
                continue;
            }

            probabilities.push_back(probability);
            xValues.push_back(xValue);
        }
        // print table
        for (int i = 0; i <probabilities.size(); i++ ) {
            std::cout << "x" << xValue << "probbality" << probabilities[i]<< std::endl;
        }
        windowSizeSize_ = probabilities.size();
        fileStream.close();
        std::cout << " file reading is done" <<std::endl;

    }

    u_int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::uniform_real_distribution<> distrib(0, 1);
        double zValue = 0;
        while (zValue == 0) {
            zValue = distrib(gen);
        }
        // binary search
        uint64_t  min = 1, max = windowSizeSize_, mid;
        do {
            mid =std::floor((min+max) / 2);
            if (probabilities[mid] >= zValue && probabilities[mid - 1] < zValue) {
                lastKeyIndex_ = mid;
                return xValues[mid];
            } else if(  probabilities[mid] >= zValue ) {
                max = mid - 1;
            } else {
                min = mid + 1;
            }
        } while ( min <= max);
        lastKeyIndex_ = windowSizeSize_;
        return xValues[lastKeyIndex_ - 1];  // fixme
    }



private:
    double  c_;
    uint64_t windowSizeSize_;
    std::vector<double> probabilities;
    std::vector<uint64_t> xValues;
    uint64_t lastKeyIndex_;
};

class ExponentialDistributionWindowLength : public WindowLength{
public:
    explicit ExponentialDistributionWindowLength(double lambda) : lambda_(lambda) {}

    u_int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::exponential_distribution<> distrib(lambda_);
        u_int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double  lambda_;
};


class NormalDistributionWindowLength: public WindowLength {
public:
    explicit NormalDistributionWindowLength(double mean, double sd): mean_(mean), sd_(sd) {}

    u_int64_t Next() override {
        std::random_device rd;
        std::mt19937 gen (rd());
        std::normal_distribution<>  distrib(mean_, sd_);
        u_int64_t len = std::max(round(distrib(gen)), 1.0);
        return len;
    }

private:
    double mean_;
    double sd_;
};


class WindowLengthBuilder {
public:
    static std::optional<std::shared_ptr<WindowLength>> BuildWindowLength(uint16_t distType, std::shared_ptr<DistributionParameters>  params) {
        if (distType == WindowLength::UniformRealDistrib ) {
            return std::make_shared<UniformRealWindowLength>(params->a_real, params->b_real);
        } else if (distType == WindowLength::ExponentialDistrib ) {
            return std::make_shared<ExponentialDistributionWindowLength>(params->lambda);
        } else if (distType == WindowLength::NormalDistrib) {
            return std::make_shared<NormalDistributionWindowLength>(params->mean, params->sd);
        }  else if (distType == WindowLength::ConstantDistrib) {
            return std::make_shared<ConstantWindowLength>(params->constantInt);
        }

        return {}; // the distType is not valid
    }
};
#endif //GADGET_WINDOWLENGTH_H
