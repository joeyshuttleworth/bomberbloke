#include <math.h>
#include <stdexcept>
#include <sstream>

#include "AbstractCollider.hpp"

double vectorProduct(dvector vec1, dvector vec2) {
    double dot = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    return dot;
}

double vectorNorm(dvector vec) {
    double normVal = vec[0] * vec[0] + vec[1] * vec[1];
    normVal = sqrt(normVal);
    return normVal;
}

AbstractCollider::AbstractCollider() {}

AbstractCollider::AbstractCollider(std::vector<dvector> vertices) {
    mFrameVertices = vertices;
}

std::array<double, 2> AbstractCollider::projectOntoAxis(dvector axis) {
    double minValue = 0.;
    double maxValue = 0.;
    
    if (mFrameVertices.size() > 0) {
        minValue = vectorProduct(mFrameVertices[0], axis);
        maxValue = minValue;
        // Project all vertices onto the axis
        // Obtain minimum and maximum projection values
        for (unsigned int i = 1; i < mFrameVertices.size(); i++) {
            double projectionValue = vectorProduct(mFrameVertices[i], axis);
            if (projectionValue < minValue)
                minValue = projectionValue;
            if (projectionValue > maxValue)
                maxValue = projectionValue;
        }
    }

    // Project position vector onto axis
    double positionProjection = vectorProduct(mPosition, axis);

    // Normalise values to calculate projection for normalised axis
    // TODO: Remove the normalisation step to improve efficiency
    double axisNorm = vectorNorm(axis);

    // Add position projection and min/max values to obtain interval of
    // projected collider
    std::array<double, 2> retVal = {{
        (minValue + positionProjection) / axisNorm,
        (maxValue + positionProjection) / axisNorm
    }};

    return retVal;
}

std::array<double, 2> AbstractCollider::testNormalAxes(std::shared_ptr<AbstractCollider> collider) {
    double minTransMagnitude = -1;
    std::array<double, 2> minTransDirection= {{0., 0.}};

    // Test all axes normal to edges of the frame
    // TODO: Skip axes from edges on the other side of the frame
    int nVertices =  mFrameVertices.size();
    for (int i = 0; i < nVertices; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::array<double, 2> axis = {{
            mFrameVertices[i][1] - mFrameVertices[(i + 1) % nVertices][1],
            mFrameVertices[(i + 1) % nVertices][0] - mFrameVertices[i][0]
        }};

        std::array<double, 2> selfProjVal = projectOntoAxis(axis);
        std::array<double, 2> colliderProjVal = collider->projectOntoAxis(axis);

        // Check if the projections intersect
        if (selfProjVal[1] > colliderProjVal[0] && selfProjVal[0] < colliderProjVal[1]) {
            // Intersection with collider projection on the right
            if (selfProjVal[1] - colliderProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = selfProjVal[1] - colliderProjVal[0];
                minTransDirection = axis;
            }
        } else if (colliderProjVal[1] > selfProjVal[0] && colliderProjVal[0] < selfProjVal[1]) {
            // Intersection with collider projection on the left
            if (colliderProjVal[1] - selfProjVal[0] < minTransMagnitude || minTransMagnitude == -1) {
                minTransMagnitude = colliderProjVal[1] - selfProjVal[0];
                minTransDirection = {{-axis[0], -axis[1]}};
            }
        } else {
            // No intersection detected - separating axis obtained
            return {{0.0, 0.0}};
        }
    }

    // Compute minimum translational vector pointing away from self
    double directionNorm = vectorNorm(minTransDirection);
    std::array<double, 2> retVal = {{
        minTransDirection[0] * minTransMagnitude / directionNorm,
        minTransDirection[1] * minTransMagnitude / directionNorm
    }};

    return retVal;
}
