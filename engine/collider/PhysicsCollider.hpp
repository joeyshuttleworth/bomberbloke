#ifndef PHYSICS_COLLIDER_HPP
#define PHYSICS_COLLIDER_HPP

#include "AbstractCollider.hpp"

class PhysicsCollider: public AbstractCollider {
public:
    dvector mVelocity = {{0., 0.}};
    double mMass;
    
    int getType() const;
    void resolveCollision(std::shared_ptr<AbstractCollider> collider, dvector mvt);
};

#endif
