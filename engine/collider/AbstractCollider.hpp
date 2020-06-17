#ifndef ABSTARCT_COLLIDER_HPP
#define ABSTARCT_COLLIDER_HPP

#include <iostream>
#include <utility>
#include <array>
#include <vector>

using dvector = std::array<double, 2>;

double vectorProduct(dvector vec1, dvector vec2);
double vectorNorm(dvector vec);

class AbstractCollider {
public:
    enum colliderType {PHYSICS, STATIC};
    dvector mPosition;
    
    // Use counter clockwise vertices of convex shape
    std::vector<dvector> mFrameVertices;

    std::array<double, 2> projectOntoAxis(dvector axis);
    std::array<double, 2> testNormalAxes(std::shared_ptr<AbstractCollider> collider);
    
    virtual int getType() const = 0;
    virtual void resolveCollision(std::shared_ptr<AbstractCollider> collider, dvector mvt) = 0;

    AbstractCollider();
    AbstractCollider(std::vector<dvector> vertices);
};

#endif
