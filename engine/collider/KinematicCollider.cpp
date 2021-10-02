#include <math.h>

#include "KinematicCollider.hpp"

void
KinematicCollider::registerCollision(std::shared_ptr<AbstractCollider> collider,
                                     dvector mtv)
{
  // If the collider isn't moving it isn't affected by the collision
  if (mVelocity[0] == 0 && mVelocity[1] == 0)
    return;

  // Magnitude of velocity in the positive MTV direction
  double mtvVelocity = vectorProduct(mVelocity, mtv);
  mtvVelocity = (mtvVelocity + fabs(mtvVelocity)) / 2;

  // Magnitude of velocity in the negative MTV direction from collider
  double colliderMtvVelocity = vectorProduct(collider->mVelocity, mtv);
  colliderMtvVelocity = (-colliderMtvVelocity + fabs(colliderMtvVelocity)) / 2;

  // A translation must occur along the MTV to resolve the collision
  // Compute the proportion of the translation undergone by self
  double translationProportion;
  double totalMtvSpeed = mtvVelocity + colliderMtvVelocity;
  if (totalMtvSpeed > 0) {
    // Share translation with collider depending on velocity in MTV direction
    translationProportion = mtvVelocity / totalMtvSpeed;
  } else if (collider->mVelocity[0] == 0 || collider->mVelocity[1] == 0) {
    // collider is stationary (in MTV direction) and so it cannot be
    // translated
    translationProportion = 1.0;
  } else {
    // Both are moving perpendicular to MTV, split translation equally
    // between the two
    translationProportion = 0.5;
  }

  double mtvSqrNorm = vectorProduct(mtv, mtv);

  // Compute required change in DeltaPosition
  double deltaDeltaPosition =
    translationProportion + vectorProduct(mDeltaPosition, mtv) / mtvSqrNorm;
  deltaDeltaPosition = fmax(deltaDeltaPosition, 0);
  // Apply change in DeltaPosition
  mDeltaPosition[0] -= mtv[0] * deltaDeltaPosition;
  mDeltaPosition[1] -= mtv[1] * deltaDeltaPosition;

  // Compute change in velocity
  // Remove velocity in positive MVT direction
  if (mDeltaVelocity[0] == 0 && mDeltaVelocity[1] == 0) {
    mDeltaVelocity[0] = -mtvVelocity * mtv[0] / mtvSqrNorm;
    mDeltaVelocity[1] = -mtvVelocity * mtv[1] / mtvSqrNorm;
  } else if (mtvVelocity > 0) {
    // If MTV is in a different direction to DeltaVelocity, set velocity
    // to zero
    double deltaVelocitySqrNorm = vectorProduct(mDeltaVelocity, mDeltaVelocity);
    double sqrProd = pow(vectorProduct(mDeltaVelocity, mtv), 2);
    if (sqrProd != mtvSqrNorm * deltaVelocitySqrNorm) {
      // delta velocity is in a different direction to MTV
      mDeltaVelocity[0] = -mVelocity[0];
      mDeltaVelocity[1] = -mVelocity[1];
    }
  }
}

void
KinematicCollider::resolveCollision()
{
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
