#ifndef GAUSS_FILTER_H
#define GAUSS_FILTER_H

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <QtGlobal>

/* https://www.johndcook.com/blog/cpp_phi/ */
static double phi(double x)
{
    // constants
    const double a1 =  0.254829592;
    const double a2 = -0.284496736;
    const double a3 =  1.421413741;
    const double a4 = -1.453152027;
    const double a5 =  1.061405429;
    const double p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = std::fabs(x) / std::sqrt(2.0);

    // A&S formula 7.1.26
    double t = 1.0 / (1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*std::exp(-x*x);

    return 0.5 * (1.0 + sign*y);
}

static double phi(double x, double stddev) {
    return phi(x / stddev);
}

/* filterSize MUST be uneven, ie. 3x3, 5x5, 7x7 etc. is supported,
 * but 4x4, 6x6 etc. is not */
std::vector<double> generate1DGauss(unsigned filterSize) {
    Q_ASSERT(filterSize % 2 == 1);

    std::vector<double> normCoeffs;

    /* numCoeffs is the number of UNIQUE coefficients,
     * ie. 3x3 -> 2, 5x5 -> 3 and so on, ignoring the
     * symmetric duplicates on the other side */
    const int numCoeffs = filterSize/2 + 1;
    /* cutOff is the x at which we assume the gaussian
     * function to reach the x-axis; ie. phi(cutOff) ~= 1 */
    const double cutOff = 3.0;
    /* Calculate standard derivative that maps the filter size
     * so the gaussian integral roughly covers them */
    const double stddev = (numCoeffs - 0.5) / cutOff;

    double lastVal = 0;

    for (int i = 0; i < numCoeffs; ++i) {
        const double x = i - numCoeffs + 1.5;
        const double phiX = phi(x, stddev);

        normCoeffs.push_back(phiX - lastVal);
        lastVal = phiX;
    }

    /* Wrap this part of the code in a lambda so we can both use
     * it directly in an assert (gets optimized out in Rlease mode)
     * and easily mark it as potentially unused */
    const auto calcNormSum = [&]() -> double {
        std::vector<double> tmp = normCoeffs;
        std::reverse(tmp.begin(), tmp.end());
        double normSum = std::accumulate(tmp.begin()+1, tmp.end(), 0.0);
        normSum = normSum * 2 + tmp.front();
        return normSum;
    };

    /* Check that integral areas add up to 1.0 */
    Q_UNUSED(calcNormSum);
    Q_ASSERT(qAbs(calcNormSum() - 1.0) < 0.001);

    /* Mirror duplicate coefficients (except center one) onto
     * other side, ie. [1 4 6] -> [1 4 6 4 1], for less confusion */
    std::vector<double> result = normCoeffs;
    for (auto i = normCoeffs.crbegin()+1; i != normCoeffs.crend(); ++i) {
        result.push_back(*i);
    }

    return result;
}

#endif // GAUSS_FILTER_H
