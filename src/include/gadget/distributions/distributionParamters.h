//
// Created by Showan Asyabi on 3/4/21.
//

#ifndef GADGET_DISTRIBUTIONPARAMTERS_H
#define GADGET_DISTRIBUTIONPARAMTERS_H

class DistributionParameters {
public:
    DistributionParameters() {
        constantInt = 0;
        constantReal = 0;
        constantString = "";
        lambda = 0;
        mean = 0;
        sd = 0;
        a_real = 0;
        b_real = 0;
        a_int = 0;
        b_real = 0;
        a_int = 0;
        b_int = 0;
        s = 1;
        hotSpotFraction = 0.5;
        hotOperationFraction = 0.5;
        pathToFile = "";

    }
    int64_t constantInt;
    double  constantReal;
    std::string constantString;
    /***
     * lambda is used for poisson distribution
     */
    double  lambda;
    /***
     * mean is used for normal distribution
     */
    double  mean;
    /***
    * sd is used for normal distribution
    */
    double sd;
    /***
    * a is used for Real uniform distribution
    */
    double  a_real;

    /***
     * b is used for Real uniform distribution
     */
    double  b_real;

    /***
    * a is used for integer uniform distribution
    */
    int64_t  a_int;

    /***
     * b is used for integer uniform distribution
     */
    int64_t   b_int;

    /***
    * p  is used for geometric distribution
    */
    double  p;

    /***
    * s is used for zipf distribution
    */
    double  s;

    /***
    * hotSpotFraction is used for hot spot   distribution
     * where a portion (hotSpotFraction) of  keys are accessed
     * hotOperationFraction of times
    */
    double  hotSpotFraction;

    double  hotOperationFraction;


    /**
     * To read keys or ECDF from a file
     */
    std::string  pathToFile;
};

#endif //GADGET_DISTRIBUTIONPARAMTERS_H
