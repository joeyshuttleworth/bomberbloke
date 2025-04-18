#ifndef NAVGRID_HPP
#define NAVGRID_HPP

#include "actor.hpp"
#include "scene.hpp"
#include "StaticCollider.hpp"
#include <utility>
#include <type_traits>
#include <typeinfo>

class NavGrid{
protected:
  double mGridSize = 1.0;
  std::shared_ptr<scene> pScene;
  std::vector<std::type_index> mBlockingActorTypes = {};

public:

  std::vector<std::pair<int, int>> mNodes = {};

  void init(){}
  void update();
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

  void ComputeGrid(){
    int size_x = std::ceil(pScene->mDimmension[0] / mGridSize);
    int size_y = std::ceil(pScene->mDimmension[1] / mGridSize);

    auto actor_list = pScene->mActors;
    /* The instances of the mBlockingActorTypes which are actually in the level */
    auto end_it =
      std::remove_if(
                     actor_list.begin(),
                     actor_list.end(),
                     [&](std::shared_ptr<actor> a)
                     {
                       return std::find(mBlockingActorTypes.begin(),
                                        mBlockingActorTypes.end(),
                                        typeid(*a.get()))
                         != mBlockingActorTypes.end();
                     }
                     );

    actor_list.resize(std::distance(
                                    actor_list.begin(),
                                    end_it
                                    )
                      );

    auto blocking_actors = actor_list;

    std::vector<dvector> vertices{{1, 0}, {1, 1}, {0, 1}, {0, 0}};
    StaticCollider test_square(vertices);

    for(int x = 0; x < size_x; x++){
      test_square.mFrameVertices[0][0] += mGridSize;
      test_square.mFrameVertices[1][0] += mGridSize;
      test_square.mFrameVertices[2][0] += mGridSize;
      test_square.mFrameVertices[3][0] += mGridSize;

      for(int i = 0; i < 4; i++)
        test_square.mFrameVertices[i][1] = 0;

      for(int y = 0; y < size_y; y++){
        test_square.mFrameVertices[0][1] += mGridSize;
        test_square.mFrameVertices[1][1] += mGridSize;
        test_square.mFrameVertices[2][1] += mGridSize;
        test_square.mFrameVertices[3][1] += mGridSize;

        auto it = std::find_if(blocking_actors.begin(), blocking_actors.end(),
                               [&](auto a){return collides(a.get(), &test_square);}
                               );
        /* Node is not blocked, add it to the grid */
        if(it == blocking_actors.end()){
          mNodes.push_back(std::pair<int, int>{x, y});
        }
      }
    }
  };

};

#endif
