#include <math.h>
#include <stdexcept>
#include <sstream>

#include "AbstractCollider.hpp"

double vectorProduct(dvector vec1, dvector vec2) {
    /**
     * Computes dot product of two double vectors
     */
    double dot = vec1[0] * vec2[0] + vec1[1] * vec2[1];

    return dot;
}

double vectorNorm(dvector vec) {
    /**
     * Computes Euclidean norm of double vector
     */
    double normVal = vec[0] * vec[0] + vec[1] * vec[1];
    normVal = sqrt(normVal);

    return normVal;
}

AbstractCollider::AbstractCollider() {}

AbstractCollider::AbstractCollider(std::vector<dvector> vertices) {
    mFrameVertices = vertices;
}

std::array<double, 2> AbstractCollider::projectOntoAxis(dvector axis) {
    /**
     * Projects collider frame onto axis, returns interval as a pair of doubles
     */
    double minValue = vectorProduct(mFrameVertices[0], axis);
    double maxValue = minValue;
   // Project onto axis and find min and max values
    for (unsigned int i = 1; i < mFrameVertices.size(); i++) {
        double projectionValue = vectorProduct(mFrameVertices[i], axis);
        if (projectionValue < minValue)
            minValue = projectionValue;
        if (projectionValue > maxValue)
            maxValue = projectionValue;
    }

    // TODO: Change mPosition to dvector
    double positionProjection = vectorProduct(mPosition, axis);

    // Normalise values to calcualte projection for normalised axis
    double axisNorm = vectorNorm(axis);

    std::array<double, 2> retVal = {{
        (minValue + positionProjection) / axisNorm,
        (maxValue + positionProjection) / axisNorm
    }};

    return retVal;
}

std::array<double, 2> AbstractCollider::testNormalAxes(std::shared_ptr<AbstractCollider> collider) {
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

        if (selfProjVal[1] > colliderProjVal[0] && selfProjVal[0] < colliderProjVal[1]) {
            if (selfProjVal[1] - colliderProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = selfProjVal[1] - colliderProjVal[0];
                minTransDirection = axis;
            }
        } else if (colliderProjVal[1] > selfProjVal[0] && colliderProjVal[0] < selfProjVal[1]) {
            if (colliderProjVal[1] - selfProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = colliderProjVal[1] - selfProjVal[0];
                minTransDirection = {{-axis[0], -axis[1]}};
            }
        } else {
            // Separating axis obtained
            return {{0.0, 0.0}};
        }
    }

    double directionNorm = vectorNorm(minTransDirection);
    // Flip x-direction such that it points in the direction that self must
    // move to correct the collision
    std::array<double, 2> retVal = {{
        minTransDirection[0] * minTransMagnitude / directionNorm,
        minTransDirection[1] * minTransMagnitude / directionNorm
    }};

    return retVal;
}
