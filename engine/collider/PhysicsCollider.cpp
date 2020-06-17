#include <math.h>

#include "PhysicsCollider.hpp"

void PhysicsCollider::resolveCollision(std::shared_ptr<AbstractCollider> collider, dvector mvt) {
    /**
     * Resolves collision according to the minimum translation vector (MVT)
     */
    
    double mvtVelocity = vectorProduct(mVelocity, mvt);
    double speedProportion = 1.;
    
    // In case of colliding with physics collider
    if (collider->getType() == PHYSICS) {
        // Share translation with collider depending on velocity in MVT direction
        double totalMvtSpeed = fabs(mvtVelocity) + vectorProduct(mVelocity, mvt);
        if (totalMvtSpeed > 0) {
            speedProportion = fabs(mvtVelocity) / totalMvtSpeed;
        } else {
            // Neither are moving - split translation equally
            speedProportion = 0.5;
        }
    }
    
    // Correct position
    mPosition[0] -= mvt[0] * speedProportion;
    mPosition[1] -= mvt[1] * speedProportion;
    
    // Correct velocity
    mVelocity[0] -= mvtVelocity * mvt[0];
    mVelocity[1] -= mvtVelocity * mvt[1];
}

int PhysicsCollider::getType() const {
    return PHYSICS;
}