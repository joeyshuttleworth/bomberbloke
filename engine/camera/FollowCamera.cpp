#include "FollowCamera.hpp"
#include "actor.hpp"

#include <math.h>

void FollowCamera::update() {
    Camera::update();

    std::shared_ptr<actor> subjectPtr = mSubject.lock();
    if (subjectPtr) {
        double distance = std::pow(std::pow(subjectPtr->mPosition[0] - mPosition[0], 2)
            + std::pow(subjectPtr->mPosition[1] - mPosition[1], 2), 0.5);

        if (distance > CAMERA_MAX_DISTANCE) {
            // If the camera distance exceeds the maximum move straight to actor
            mVelocity[0] = 0;
            mVelocity[1] = 0;
            mPosition[0] = subjectPtr->mPosition[0];
            mPosition[1] = subjectPtr->mPosition[1];
        } else if (distance == 0) {
            // Set velocity to zero if on actor
            mVelocity[0] = 0;
            mVelocity[1] = 0;
        } else {
            double speed = std::sqrt(std::pow(mVelocity[0], 2) + std::pow(mVelocity[1], 2));

            // Accelerate towards bloke
            speed += CAMERA_ACCELERATION / TICK_RATE;

            if (speed > CAMERA_MAX_SPEED) {
                // Speed has exceeded maximum
                speed = CAMERA_MAX_SPEED;
            }

            if (std::pow(speed, 2) > 2 * distance * CAMERA_ACCELERATION) {
                // Decelerate on to actor
                speed = std::pow(2 * distance * CAMERA_ACCELERATION, 0.5);
            }

            if (distance < speed / TICK_RATE) {
                // Close enough - move onto subject
                mVelocity[0] = 0;
                mVelocity[1] = 0;
                mPosition[0] = subjectPtr->mPosition[0];
                mPosition[1] = subjectPtr->mPosition[1];
            } else {
                // Point velocity in direction of subject
                mVelocity[0] = (subjectPtr->mPosition[0] - mPosition[0]) / distance * speed;
                mVelocity[1] = (subjectPtr->mPosition[1] - mPosition[1]) / distance * speed;
                mPosition[0] += mVelocity[0] / TICK_RATE;
                mPosition[1] += mVelocity[1] / TICK_RATE;
            }
        }
    } else if (mVelocity[0] != 0 || mVelocity[1] != 0) {
        double speed = std::sqrt(std::pow(mVelocity[0], 2) + std::pow(mVelocity[1], 2));

        // Camera isn't attached to any subject - decelerate to 0 velocity
        if (CAMERA_ACCELERATION / TICK_RATE < speed) {
            mVelocity[0] -= mVelocity[0] / speed * CAMERA_ACCELERATION / TICK_RATE;
            mVelocity[1] -= mVelocity[1] / speed * CAMERA_ACCELERATION / TICK_RATE;
            mPosition[0] += mVelocity[0] / TICK_RATE;
            mPosition[1] += mVelocity[1] / TICK_RATE;
        } else {
            mVelocity[0] = 0;
            mVelocity[1] = 0;
        }
    }
}
