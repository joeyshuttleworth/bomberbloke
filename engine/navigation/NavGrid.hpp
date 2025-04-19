#ifndef NAVGRID_HPP
#define NAVGRID_HPP

#include "actor.hpp"
#include "scene.hpp"
#include "StaticCollider.hpp"
#include <utility>
#include <type_traits>
#include <typeinfo>

using ivector = std::array<int, 2>;

class NavGrid{
protected:
  double mGridSize = 1.0;
  std::shared_ptr<scene> pScene;
  std::vector<std::type_index> mBlockingActorTypes = {};

public:

  std::vector<ivector> mNodes = {};

  template <typename Container>
  NavGrid(Container blocking_actor_list, std::shared_ptr<scene> _pScene,
          double grid_size = 1.0
          ){
    pScene = _pScene;
    mGridSize = grid_size;
    for(auto i = blocking_actor_list.begin(); i != blocking_actor_list.end(); i++){
      mBlockingActorTypes.push_back(*i);
    }
    return;
  }

  std::vector<ivector> getNeighours(ivector node);

  std::vector<ivector> getConnectedComponentFromNode(ivector);

  void computeGrid();

#endif
