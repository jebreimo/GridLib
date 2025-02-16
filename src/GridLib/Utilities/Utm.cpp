#include <cmath>
#include "Utm.hpp"

namespace GridLib
{
    template <size_t N>
    static void compute_powers(double (&arr)[N])
    {
        for (size_t i = 1; i < N; i++)
            arr[i] = arr[i - 1] * arr[0];
    }

    SphericalCoords utm_to_geo(double easting, double northing, int utm_zone)
    {
        constexpr double pi = 3.14159265358979323846264338328;
        constexpr double f =  1 / 298.257222101;
        constexpr double k0 = 0.9996;
        constexpr double a  = 6378137;
        constexpr double ecc_squared = 2.0 * f - f * f;
        constexpr double ecc_prime_squared = ecc_squared / (1 - ecc_squared);
        double e[4] = {(1. - std::sqrt(1. - ecc_squared)) / (1.0 + std::sqrt(1.0 - ecc_squared))};
        compute_powers(e);

        const double m = northing / k0;
        const double mu = m / (a * (1 - ecc_squared * (1 + ecc_squared * (3 + 5 * ecc_squared / 4) / 16) / 4));
        const double phi1_rad = mu + (  3 * e[0] /  2 - 27 * e[2] / 32) * std::sin(2 * mu)
                                   + ( 21 * e[1] / 16 - 55 * e[3] / 32) * std::sin(4 * mu)
                                   + (151 * e[2] / 96)                  * std::sin(6 * mu);

        const double n1 = a / std::sqrt(1 - ecc_squared * std::sin(phi1_rad) * std::sin(phi1_rad));
        const double r1 = a * (1 - ecc_squared) / std::pow(1 - ecc_squared * std::sin(phi1_rad) * std::sin(phi1_rad), 1.5);

        double t[2] = {std::tan(phi1_rad) * std::tan(phi1_rad)};
        compute_powers(t);

        double c[2] = {ecc_prime_squared * std::cos(phi1_rad) * std::cos(phi1_rad)};
        compute_powers(c);

        constexpr double false_east = 500000; // remove 500,000 meter offset for longitude
        double d[6] = {(easting - false_east) / (n1 * k0)};
        compute_powers(d);

        const double lat = phi1_rad - (n1 * std::tan(phi1_rad) / r1) * (
                           d[1] / 2.0 -
                           ( 5 +  3 * t[0] +  10 * c[0] -  4 * c[1] -   9 * ecc_prime_squared)            * d[3] /  24 +
                           (61 + 90 * t[0] + 298 * c[0] + 45 * t[1] - 252 * ecc_prime_squared - 3 * c[1]) * d[5] / 720);
        const double lon_origin = (6 * utm_zone - 3 - 180) * pi / 180;  // -3 puts origin in middle of zone
        const double lon = lon_origin +
                           (d[0] -
                            (1 +  2 * t[0] +     c[0])                                                * d[2] /   6 +
                            (5 + 28 * t[0] - 2 * c[0] - 3 * c[1] + 8 * ecc_prime_squared + 24 * t[1]) * d[4] / 120
                           ) / std::cos(phi1_rad);

        return {lat * 180 / pi, lon * 180 / pi};
    }
}
