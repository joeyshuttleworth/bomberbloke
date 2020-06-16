#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <utility>
#include <vector>

using matrix = std::vector<std::vector<double>>;
using dvector = std::vector<double>;

class ColliderFrame {
public:
    matrix mFrameVertices;
    
    std::pair<double, double> projectOntoAxis(dvector axis, dvector centre);

    ColliderFrame();
    ColliderFrame(matrix vertices);
};

#endif