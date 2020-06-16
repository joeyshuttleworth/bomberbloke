#include <cmath>

#include "ColliderFrame.hpp"

double _vectorProduct(dvector vec1, dvector vec2) {
    /**
     * Computes dot product of two double vectors
     */

    // TODO: Handle erroneous case where dim v1 =/= dim v2
    // if (vec1.size() != vec2.size()) {
    //     return;
    // }

    double dot = 0;
    for (unsigned int i = 0; i < vec1.size(); i++) {
        dot += vec1[i] * vec2[i];
    }

    return dot;
}

double _vectorNorm(dvector vec) {
    /**
     * Computes Euclidean norm of double vector
     */
    double normVal = 0;
    for (unsigned int i = 0; i < vec.size(); i++) {
        normVal += vec[i] * vec[i];
    }
    normVal = std::sqrt(normVal);

    return normVal;
}

ColliderFrame::ColliderFrame() {}

ColliderFrame::ColliderFrame(matrix vertices) {
    mFrameVertices = vertices;
}

std::pair<double, double> ColliderFrame::projectOntoAxis(dvector axis, dvector position) {
    /**
     * Projects collider frame onto axis, returns interval as a pair of doubles
     */
    double minValue = _vectorProduct(mFrameVertices[0], axis);
    double maxValue = minValue;
   // Project onto axis and find min and max values
    for (int i = 1; i < mFrameVertices.size(); i++) {
        double projectionValue = _vectorProduct(mFrameVertices[i], axis);
        if (projectionValue < minValue)
            minValue = projectionValue;
        if (projectionValue > maxValue)
            maxValue = projectionValue;
    }

    double positionProjection = _vectorProduct(position, axis);

    // Normalise values to calcualte projection for normalised axis
    double axisNorm = _vectorNorm(axis);

    std::pair<double, double> retVal(
        (minValue + positionProjection) / axisNorm,
        (maxValue + positionProjection) / axisNorm
    );

    return retVal;
}
