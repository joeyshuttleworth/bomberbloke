#include <cmath>
#include <stdexcept>
#include <iostream>
#include "ColliderFrame.hpp"
#include <sstream>

double _vectorProduct(dvector vec1, dvector vec2) {
    /**
     * Computes dot product of two double vectors
     */

    if (vec1.size() != vec2.size()) {
        #ifdef __GNUG__
        std::stringstream msg;
        msg << "Vectors different sizes in " << std::string(__func__) << std::endl;
        #else
        msg << "Vectors different sizes in " << std::string(__PRETTY_FUNCTION__) << std::endl;
        #endif
        throw std::invalid_argument(msg.str());
    }

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

ColliderFrame::ColliderFrame(std::vector<dvector> vertices) {
    mFrameVertices = vertices;
}

std::array<double, 2> ColliderFrame::projectOntoAxis(dvector axis) {
    /**
     * Projects collider frame onto axis, returns interval as a pair of doubles
     */
    double minValue = _vectorProduct(mFrameVertices[0], axis);
    double maxValue = minValue;
   // Project onto axis and find min and max values
    for (unsigned int i = 1; i < mFrameVertices.size(); i++) {
        double projectionValue = _vectorProduct(mFrameVertices[i], axis);
        if (projectionValue < minValue)
            minValue = projectionValue;
        if (projectionValue > maxValue)
            maxValue = projectionValue;
    }

    // TODO: Change mPosition to dvector
    dvector position = {{mPosition[0], mPosition[1]}};
    double positionProjection = _vectorProduct(position, axis);

    // Normalise values to calcualte projection for normalised axis
    double axisNorm = _vectorNorm(axis);

    std::array<double, 2> retVal = {{
        (minValue + positionProjection) / axisNorm,
        (maxValue + positionProjection) / axisNorm
    }};

    return retVal;
}

std::array<double, 2> ColliderFrame::testNormalAxes(std::shared_ptr<ColliderFrame> collider) {
    double minTransMagnitude = -1;
    std::array<double, 2> minTransDirection= {{0.0, 0.0}};

    int nVertices =  mFrameVertices.size();
    for (int i = 0; i < nVertices; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::array<double, 2> axis = {{
            mFrameVertices[i][1] - mFrameVertices[i+1 % nVertices][1],
            mFrameVertices[i+1 % nVertices][0] - mFrameVertices[i][0]
        }};

        std::array<double, 2> selfProjVal = projectOntoAxis(axis);
        std::array<double, 2> colliderProjVal = collider->projectOntoAxis(axis);

        // std::cout << " " << std::endl;
        // std::cout << selfProjVal[0] << " " << selfProjVal[1] << std::endl;
        // std::cout << colliderProjVal[0] << " " << colliderProjVal[1] << std::endl;

        if (selfProjVal[1] > colliderProjVal[0] && selfProjVal[0] < colliderProjVal[1]) {
            // std::cout << "in 1" << std::endl;
            if (selfProjVal[1] - colliderProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = selfProjVal[1] - colliderProjVal[0];
                minTransDirection = axis;
            }
        } else if (colliderProjVal[1] > selfProjVal[0] && colliderProjVal[0] < selfProjVal[1]) {
            // std::cout << "in 2" << std::endl;
            if (colliderProjVal[1] - selfProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = colliderProjVal[1] - selfProjVal[0];
                minTransDirection = {{-axis[0], -axis[1]}};
            }
        } else {
            // Separating axis obtained
            return {{0.0, 0.0}};
        }
    }

    double directionNorm = _vectorNorm(minTransDirection);
    // Flip x-direction such that it points in the direction that self must
    // move to correct the collision
    std::array<double, 2> retVal = {{
        minTransDirection[0] * minTransMagnitude / directionNorm,
        minTransDirection[1] * minTransMagnitude / directionNorm
    }};

    return retVal;
}
