#include "PhysicsCollider.hpp"

void PhysicsCollider::resolveCollision(std::shared_ptr<AbstractCollider> collider, dvector mvt) {
    // Nothing changes for static collider on collision
}

int PhysicsCollider::getType() const {
    return STATIC;
}