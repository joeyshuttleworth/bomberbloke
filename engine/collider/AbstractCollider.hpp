#ifndef ABSTRACTCOLLIDER_HPP
#define ABSTRACTCOLLIDER_HPP

#include <iostream>
#include <utility>
#include <array>
#include <vector>
#include <memory>

using dvector = std::array<double, 2>;

/**
 * Computes dot product between two vectors
 */
double vectorProduct(dvector vec1, dvector vec2);

/**
 * Computes Euclidean norm of double vector
 */
double vectorNorm(dvector vec);

class AbstractCollider {
public:
    enum colliderType {KINEMATIC, STATIC};
    /**
     * Position and velocity are represented as a 2-array of doubles.
     */
    dvector mPosition;
    dvector mVelocity = {{0., 0.}};

    /**
     * The vertices that form the shape of the collider.
     * Must be convex. List vertices counter-clockwise.
     * TODO: support circles
     */
    std::vector<dvector> mFrameVertices;

    /**
     * Helper function for SAT computations. Projects collision frame onto axis.
     * Returns interval of projection as minimum and maximum value (in that
     * order).
     */
    std::array<double, 2> projectOntoAxis(dvector axis);

    /**
     * Tests for a separating axis on all axes normal to the frame.
     * If found to be non-separating, returns the smallest translation vector
     * that resolves the collision and is in the direction of one of the axes
     * described above.
     * By standard, vectors are given that point away from the collider for
     * which the method is called.
     */
    std::array<double, 2> testNormalAxes(
        std::shared_ptr<AbstractCollider> collider
    );

    /**
     * Registers a collision between itself and some other collider.
     * Does not make any changes to physical states.
     */
    virtual void registerCollision(
        std::shared_ptr<AbstractCollider> collider, 
        dvector mtv
    ) = 0;

    /**
     * Applies changes resulting from registered collisions. Called after
     * registerCollision is called for all collisions.
     */
    virtual void resolveCollision() = 0;

    /**
     * Function for obtaining collider type for derived classes.
     */
    virtual int getType() const = 0;

    AbstractCollider();
    AbstractCollider(std::vector<dvector> vertices);
};

#endif
