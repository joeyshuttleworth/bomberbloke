#ifndef KINEMATIC_COLLIDER_HPP
#define KINEMATIC_COLLIDER_HPP

#include "AbstractCollider.hpp"

class KinematicCollider: public AbstractCollider {
private:
    /**
     * Changes in velocity and position that are computed during collision
     * detection and registration phase. Outside of this phase, set to zero.
     */
    dvector mDeltaVelocity = {{0., 0.}};
    dvector mDeltaPosition = {{0., 0.}};
    
public:
    /**
     * Registers a collision between itself and some other collider.
     * Only computes correct delta values to be applied on resolveCollision.
     * Does not make any changes to position and velocity.
     */
    void registerCollision(std::shared_ptr<AbstractCollider> collider, dvector mtv);
    
    /**
     * Applies changes resulting from registered collisions. Called after
     * registerCollision is called for all collisions.
     */
    void resolveCollision();
    
    /**
     * Function for obtaining collider type.
     */
    int getType() const {
        return KINEMATIC;
    }
};

#endif
