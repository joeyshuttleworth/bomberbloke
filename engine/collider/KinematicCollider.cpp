#include <math.h>

#include "KinematicCollider.hpp"

void KinematicCollider::registerCollision(std::shared_ptr<AbstractCollider> collider, dvector mtv) {
    // Magnitude of velocity in the positive MTV direction
    double mtvVelocity = vectorProduct(mVelocity, mtv);
    mtvVelocity = (mtvVelocity + fabs(mtvVelocity)) / 2;
    
   // Magnitude of velocity in the negative MTV direction from collider
    double colliderMtvVelocity = vectorProduct(collider->mVelocity, mtv);
    colliderMtvVelocity = (-colliderMtvVelocity + fabs(colliderMtvVelocity)) / 2;

    // Share translation with collider depending on velocity in MTV direction
    double speedProportion = 1.;
    double totalMtvSpeed = mtvVelocity + colliderMtvVelocity;
    if (totalMtvSpeed > 0) {
        speedProportion = mtvVelocity / totalMtvSpeed;
    } else if(mVelocity[0] != 0 || mVelocity[1] != 0){
        // Neither are moving and this is completely
        // stationary - split translation equally
        speedProportion = 0.5;
    }
    else{
      speedProportion = 0;
    }

    // Compute change in position
    mDeltaPosition[0] = -mtv[0] * speedProportion;
    mDeltaPosition[1] = -mtv[1] * speedProportion;
    
    // Compute change in velocity
    // Remove velocity in positive MVT direction
    double mtvSqrNorm = vectorProduct(mtv, mtv);
    mDeltaVelocity[0] = -mtvVelocity * mtv[0] / mtvSqrNorm;
    mDeltaVelocity[1] = -mtvVelocity * mtv[1] / mtvSqrNorm;
}

void KinematicCollider::resolveCollision() {
    // Apply delta values to positionand velocity
    mPosition[0] += mDeltaPosition[0];
    mPosition[1] += mDeltaPosition[1];
    
    mVelocity[0] += mDeltaVelocity[0];
    mVelocity[1] += mDeltaVelocity[1];
     
    // Reset delta values to zero
    mDeltaPosition[0] = 0.;
    mDeltaPosition[1] = 0.;
    
    mDeltaVelocity[0] = 0.;
    mDeltaVelocity[1] = 0.;
}
