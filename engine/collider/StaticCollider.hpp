#ifndef STATIC_COLLIDER_HPP
#define STATIC_COLLIDER_HPP

#include "AbstractCollider.hpp"

class StaticCollider: public AbstractCollider {
public:
    /**
     * Registering and resolving colliders (see AbstractCollider.hpp).
     * This collider does not need to change anything based on collisions.
     */
    void registerCollision(std::shared_ptr<AbstractCollider> collider, dvector mtv) {};
    void resolveCollision() {};
    
    /**
     * Function for obtaining collider type.
     */
    int getType() const {
        return STATIC;
    }
};

#endif
