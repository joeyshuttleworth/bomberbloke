#include "NavGrid.hpp"

NavGrid::computeGrid(){
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
          mNodes.push_back(ivector{x, y});
        }
      }
    }
  };

static bool are_neighbours(ivector a, ivector b, bool diagonal = false){
  /* Test if two cells are neighbouring either horizontally or vertically.
     If diagonal=true, then return true if neighbours are diagonally adjacent also.
   */

  bool horizontal_n = std::abs(a[0] - b[0]) == 1;
  bool vertical_n   = std::abs(a[1] - b[1]) == 1;

  if(diagonal && horizontal_n && vertical_n){
    return true;
  }

  else if(horizontal_n != vertical_n){
      return true;
    }

  return false;
}

std::vector<ivector> NavGrid::getNeighours(ivector node, bool include_diagonal = false){
  std::vector<ivector> neighbours = mNodes;

  auto end_it = std:::remove_if(neighbours.begin(),
                                neightbours.end(),
                                [&](auto a){return are_neighbours(node, a);}
                                );
  neighbours.resize(std::distance(neighbours.begin(), end_it()));

  /* Now add diagonal squares if the adjacent ones are free */
  auto diagonal_squares = mNodes;
  auto end_it = std:::remove_if(diagonal_squares.begin(),
                                diagonal_squares.end(),
                                [&](auto a){return are_neighbours(node, a, diagonal=true) && !are_neighbours(node, a);}
                                );

  if (include_diagonal){
    for(auto it = diagonal_squares.begin(); it != end_it; i++){

      int xdiff = node[0] - *it[0];
      int ydiff = node[1] - *it[1];

      ivector adj_square_1 = {node[0] + xdiff, node[1]};
      ivector adj_square_2 = {node[0], node[1] + ydiff};

      if(std::find(neighbours.begin(), neighbours.end(), adj_square_1) != neighbours.end()){
        continue;
      }

      if(std::find(neighbours.begin(), neighbours.end(), adj_square_1) != neighbours.end()){
        continue;
      }
      neighbours.push_back(*it);
    }
  }
  return neighbours;
}

std::vector<ivector> getConnectedComponentFromNode(ivector node){
  std::vector<ivector> neighbours = this->getNeighours(node);
  std::set<ivector> to_visit(comp.begin(), comp.end());
  std::set<ivector> comp = to_visit;

  while(to_visit.size() > 0){
    for(auto it = to_visit.begin(); it != to_visit.end(); it++){
      if(!std::find(comp.begin(), comp.end(), *it)){
        std::vector<ivector> current_neighbours = this->getNeighbours(*it);
        for(auto it2 = current_neighbours.begin(); current_neighbours.end(); it2++)
          to_visit.insert(*it2);
        comp.insert
      }
    }
    if(it == to_visit.end()){
      break;
    }
  }
  std::vector<ivector> ret_vec = {};
  std::copy(comp.begin(), comp.end(), std::back_inserter(ret_vec));
  return ret_vec;
}

std::vector<std::vector<ivector>> getConnectedComponents(){
  /* Returns a vector of vectors: each connected component of the NavGrid,
     each of which is a vector of integer coordinates
   */
  std::set<ivector> visited;
  std::vector<std::vector<ivector>> return_vec = {};
  for(auto it = mNodes.begin(); it != mNodes.end(); it++){
    /* Check if node already visited */
    if(std::find(visited.begin(), visited.end(),  *it)){
      continue;
    }
    /* Add node to visited, so we don't revisit then add this component */
    else{
      /* It doesn't matter whether we set include_diagonal to true or false */
      std::vector<ivector> new_comp = getNeighbours(*it);
      for(node : new_comp)
        visited.insert(node);
    }
    return_vec.push_back(new_comp);
  }
  return return_vec;
}
