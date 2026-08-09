#ifndef NAVGRID_HPP
#define NAVGRID_HPP

#include "actor.hpp"
#include "StaticCollider.hpp"
#include <utility>
#include <set>
#include <type_traits>
#include <typeinfo>

using ivector = std::array<int, 2>;
// Hash function for ivector (std::array<int, 2>)
struct ivector_hash {
  std::size_t operator()(const ivector& v) const {
    return std::hash<int>()(v[0]) ^ (std::hash<int>()(v[1]) << 1);
  }
};

class scene;

class NavGrid{
protected:
  double mGridSize = 1.0;
  std::shared_ptr<scene> pScene;
  std::vector<std::type_index> mBlockingActorTypes = {};

public:

  std::set<ivector> mNodes = {};

  NavGrid(){
    return;
  }

  template <typename Container>
  NavGrid(Container blocking_actor_list = {}, std::shared_ptr<scene> _pScene = nullptr,
          double grid_size = 1.0
          ){
    pScene = _pScene;
    mGridSize = grid_size;
    for(auto i = blocking_actor_list.begin(); i != blocking_actor_list.end(); i++){
      mBlockingActorTypes.push_back(*i);
    }
    return;
  }

  NavGrid(const NavGrid& other){
    pScene = other.pScene;
    mGridSize = other.mGridSize;
    mBlockingActorTypes = other.mBlockingActorTypes;
    mNodes = other.mNodes;
  }

  ~NavGrid() = default;

  NavGrid& operator=(NavGrid&) = default;
  NavGrid& operator=(NavGrid&&) = default;

  std::set<ivector> getNeighbours(ivector, bool=false);
  std::set<ivector> getConnectedComponentFromNode(ivector);
  std::vector<std::set<ivector>> getConnectedComponents();
  std::vector<ivector> findRoute(ivector, ivector, bool=true);

  void computeGrid();

  std::set<ivector> getNodes(){return mNodes;}

  void addNode(ivector);
  void removeNode(ivector);
};

#endif
