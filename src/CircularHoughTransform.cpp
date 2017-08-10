//
// Created by Joshua Bradt on 7/27/17.
//

#include "CircularHoughTransform.h"

namespace attpc {
namespace cleaning {

CircularHoughTransform::CircularHoughTransform(const Eigen::Index numBins_, const double maxRadiusValue_, const Eigen::Index rowOffset_)
: HoughTransform(numBins_, maxRadiusValue_, rowOffset_)
{}

Eigen::Vector2d CircularHoughTransform::findCenter(const Eigen::ArrayXXd& data) const {
    HoughSpace hspace = findHoughSpace(data);

    Eigen::Index maxRow = 0;
    Eigen::Index maxCol = 0;
    hspace.findMaximum(maxRow, maxCol);

    // Convert max bin to angle, radius values
    const double maxAngle = hspace.findAngleFromBin(maxRow);
    const double maxRadius = hspace.findRadiusFromBin(maxCol);

    // Convert angle and radius values to positions in the data space
    Eigen::Vector2d center;
    center(0) = maxRadius * cos(maxAngle);
    center(1) = maxRadius * sin(maxAngle);

    return center;
}

double CircularHoughTransform::radiusFunction(const Eigen::ArrayXXd& data, const Eigen::Index rowIdx,
                                              const double costh, const double sinth) const {
    const double x1 = data(rowIdx, 0);
    const double y1 = data(rowIdx, 1);
    const double x0 = data(rowIdx - getRowOffset(), 0);
    const double y0 = data(rowIdx - getRowOffset(), 1);

    const double numer = (x1*x1 - x0*x0) + (y1*y1 - y0*y0);
    const double denom = 2 * ((x1 - x0) * costh + (y1 - y0) * sinth);
    return numer / denom;
}

}
}
