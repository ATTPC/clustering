//
// Created by Joshua Bradt on 7/25/17.
//

#include "catch.hpp"
#include <Eigen/Core>
#include <vector>
#include "utilities.h"

namespace {
float euclideanMetric(const Eigen::Vector3f& a, const Eigen::Vector3f& b) {
    return (a - b).norm();
}
}

TEST_CASE("Can calculate distance matrix", "[utilities]") {
    const int numPoints = 100;

    std::vector<Eigen::Vector3f> data;
    for (int i = 0; i < numPoints; ++i) {
        data.emplace_back(i, i, i);
    }

    Eigen::MatrixXf distMat = attpc::cleaning::calculateDistanceMatrix<Eigen::Vector3f>(data, euclideanMetric);

    SECTION("Distance matrix is symmetric") {
        REQUIRE(distMat == distMat.transpose());
    }

    SECTION("Distances are calculated correctly") {
        for (int i = 0; i < numPoints; ++i) {
            for (int j = 0; j < numPoints; ++j) {
                float expected = euclideanMetric(data.at(i), data.at(j));
                REQUIRE(distMat(i, j) == expected);
            }
        }
    }
}

TEST_CASE("findPeakLocations finds isolated, sharp peaks", "[utilities][findPeakLocations]") {
    const Eigen::Index numPts = 100;
    Eigen::ArrayXd testData = Eigen::ArrayXd::Zero(numPts);

    const std::vector<Eigen::Index> peakLocs = {2, 8, 25, 76};
    CAPTURE(peakLocs);

    for (const auto loc : peakLocs) {
        testData(loc) = 100;
    }

    std::vector<Eigen::Index> foundLocs = attpc::cleaning::findPeakLocations(testData, 1);
    CAPTURE(foundLocs);

    SECTION("Finds correct number of peaks") {
        REQUIRE(foundLocs.size() == peakLocs.size());
    }

    SECTION("Found peak locations are correct") {
        for (size_t i = 0; i < foundLocs.size(); ++i) {
            REQUIRE(foundLocs.at(i) == peakLocs.at(i));
        }
    }
}

TEST_CASE("findPeakLocations finds flat top peaks", "[utilities][findPeakLocations]") {
    const Eigen::Index numPts = 100;
    Eigen::ArrayXd testData = Eigen::ArrayXd::Zero(numPts);

    const std::vector<Eigen::Index> peakLocs = {5, 15, 35, 75};
    CAPTURE(peakLocs);

    for (const auto loc : peakLocs) {
        testData.segment(loc, 5) = 100;
    }

    std::vector<Eigen::Index> foundLocs = attpc::cleaning::findPeakLocations(testData, 1);
    CAPTURE(foundLocs);

    SECTION("Finds correct number of peaks") {
        REQUIRE(foundLocs.size() == peakLocs.size());
    }

    SECTION("Found peak locations are correct") {
        for (size_t i = 0; i < foundLocs.size(); ++i) {
            REQUIRE(foundLocs.at(i) == peakLocs.at(i));
        }
    }
}
