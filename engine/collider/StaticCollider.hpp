#ifndef STATIC_COLLIDER_HPP
#define STATIC_COLLIDER_HPP

#include "AbstractCollider.hpp"

class StaticCollider: public AbstractCollider {
public:
    int getType() const;
    void resolveCollision(std::shared_ptr<AbstractCollider> collider, dvector mvt);
};

#endif
