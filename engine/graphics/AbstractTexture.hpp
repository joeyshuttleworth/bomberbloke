#ifndef ABSTRACTTEXTURE_HPP
#define ABSTRACTTEXTURE_HPP

#include <array>

class IGraphicsManager;

class AbstractTexture {
public:
  AbstractTexture(IGraphicsManager* = nullptr, int=0, int=0){};

  virtual std::array<int, 2> getDimensions(){return {0, 0};};

protected:
};

#endif
