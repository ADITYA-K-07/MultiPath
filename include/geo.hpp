#pragma once
#include <cmath>

// Great-circle distance between two lat/lon points, in metres.
inline double haversine(double lat1, double lon1, double lat2, double lon2) {
    constexpr double R = 6371000.0;               // Earth radius in metres
    constexpr double DEG2RAD = 3.14159265358979323846 / 180.0;

    double dlat = (lat2 - lat1) * DEG2RAD;
    double dlon = (lon2 - lon1) * DEG2RAD;

    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1 * DEG2RAD) * std::cos(lat2 * DEG2RAD) *
               std::sin(dlon / 2) * std::sin(dlon / 2);

    return 2 * R * std::asin(std::sqrt(a));
}
