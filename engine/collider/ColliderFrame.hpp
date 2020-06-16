#ifndef COLLIDER_FRAME_HPP
#define COLLIDER_FRAME_HPP

#include <utility>
#include <array>
#include <vector>

using dvector = std::array<double, 2>;

class ColliderFrame {
public:
    // TODO: Change mPosition to dvector
    double mPosition[2];
    std::vector<dvector> mFrameVertices;

    std::array<double, 2> projectOntoAxis(dvector axis);
    std::array<double, 2> testNormalAxes(std::shared_ptr<ColliderFrame> collider);

    ColliderFrame();
    ColliderFrame(std::vector<dvector> vertices);
};

#endif
