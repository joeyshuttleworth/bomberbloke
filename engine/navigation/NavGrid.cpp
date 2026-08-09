#include <stdexcept>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "NavGrid.hpp"

void
NavGrid::computeGrid(){
    int size_x = std::ceil(pScene->mDimension[0] / mGridSize);
    int size_y = std::ceil(pScene->mDimension[1] / mGridSize);

    mNodes = {};

    std::list<std::shared_ptr<actor>> blocking_actors = pScene->mActors;
    /* The instances of the mBlockingActorTypes which are actually in the level */
    auto end_it =
      std::remove_if(
                     blocking_actors.begin(),
                     blocking_actors.end(),
                     [&](std::shared_ptr<actor> a)
                     {
                       return std::find(mBlockingActorTypes.begin(),
                                        mBlockingActorTypes.end(),
                                        typeid(*(a.get())))
                         == mBlockingActorTypes.end();
                     }
                     );

    blocking_actors.resize(std::distance(
                                    blocking_actors.begin(),
                                    end_it
                                    )
                      );

    std::cout << "No blocking actors = " << blocking_actors.size() << std::endl;

    std::vector<dvector> vertices{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    StaticCollider test_square(vertices);

    for(int x = 0; x < size_x; x++){
      test_square.mFrameVertices[0][1] = 0;
      test_square.mFrameVertices[1][1] = mGridSize;
      test_square.mFrameVertices[2][1] = mGridSize;
      test_square.mFrameVertices[3][1] = 0;
      for(int y = 0; y < size_y; y++){
        auto it = std::find_if(blocking_actors.begin(), blocking_actors.end(),
                               [&](auto a){return collides(a.get(), &test_square);}
                               );
        /* Node is not blocked, add it to the grid */
        if(it == blocking_actors.end())
          {
            mNodes.insert(ivector{x, y});
          }

        // /* Print out test squares for debugging */
        // std::cout << test_square.mFrameVertices[0][0] << ", " << test_square.mFrameVertices[0][1] << std::endl;
        // std::cout << test_square.mFrameVertices[1][0] << ", " << test_square.mFrameVertices[1][1] << std::endl;
        // std::cout << test_square.mFrameVertices[2][0] << ", " << test_square.mFrameVertices[2][1] << std::endl;
        // std::cout << test_square.mFrameVertices[3][0] << ", " << test_square.mFrameVertices[3][1] << std::endl;
        // std::cout << "==========" << std::endl;

        test_square.mFrameVertices[0][1] += mGridSize;
        test_square.mFrameVertices[1][1] += mGridSize;
        test_square.mFrameVertices[2][1] += mGridSize;
        test_square.mFrameVertices[3][1] += mGridSize;
      }
      test_square.mFrameVertices[0][0] += mGridSize;
      test_square.mFrameVertices[1][0] += mGridSize;
      test_square.mFrameVertices[2][0] += mGridSize;
      test_square.mFrameVertices[3][0] += mGridSize;
    }
    std::cout << "No nodes = " << mNodes.size() << std::endl;
  };

static bool are_neighbours(ivector a, ivector b, bool diagonal = false){
  /* Test if two cells are neighbouring either horizontally or vertically.
     If diagonal=true, then return true if neighbours are diagonally adjacent also.
   */

  bool horizontal_n = std::abs(a[0] - b[0]) == 1;
  bool vertical_n   = std::abs(a[1] - b[1]) == 1;

  unsigned int max_diff = std::max(std::abs(a[1] - b[1]), std::abs(a[0] - b[0]));

  if(max_diff > 1){
    return false;
  }

  if(diagonal && horizontal_n && vertical_n){
    return true;
  }

  else if(horizontal_n != vertical_n){
      return true;
    }

  return false;
}

static bool are_diagonal_neighbours(const ivector a, const ivector b){
  /* Test if two cells are neighbouring either horizontally or vertically.
     If diagonal=true, then return true if neighbours are diagonally adjacent also.
  */

  bool horizontal_n = std::abs(a[0] - b[0]) == 1;
  bool vertical_n   = std::abs(a[1] - b[1]) == 1;

  return horizontal_n && vertical_n;
}

std::set<ivector> NavGrid::getNeighbours(ivector node, bool include_diagonal){

  if(!mNodes.contains(node))
    throw std::invalid_argument("Node isn't present in NavGrid");

  std::set<ivector> neighbours = {};

  for(auto node2 : mNodes){
    // Check if they are vertically or horizontally adjacent
    if(are_neighbours(node, node2))
      {
        neighbours.insert(node2);
      }
  }

  /* Now add diagonal squares if the adjacent ones are free */
  if (include_diagonal)
    {
      for(auto node2 : mNodes){
        if(!are_diagonal_neighbours(node, node2))
          continue;

        ivector adj_square_1 = {node2[0], node[1]};
        ivector adj_square_2 = {node[0], node2[1]};

        if(mNodes.contains(adj_square_1) && mNodes.contains(adj_square_2))
          {
            neighbours.insert(node2);
          }
      }
    }
  return neighbours;
}

std::set<ivector> NavGrid::getConnectedComponentFromNode(ivector node){

  if (std::find(mNodes.begin(), mNodes.end(), node) == mNodes.end()){
    throw std::invalid_argument("Node not found in NavGrid");
  }

  auto neighbours = this->getNeighbours(node);

  std::set<ivector> to_visit(neighbours.begin(), neighbours.end());
  std::set<ivector> visited{node};

  auto it = to_visit.begin();
  while(to_visit.size() > 0){
    for(it = to_visit.begin(); it != to_visit.end(); it++){
      if(std::find(visited.begin(), visited.end(), *it) == visited.end()){
        auto current_neighbours = this->getNeighbours(*it);
        for(auto it2 = current_neighbours.begin(); it2!=current_neighbours.end(); it2++)
          to_visit.insert(*it2);
        visited.insert(*it);
      }
    }
    if(it == to_visit.end()){
      break;
    }
  }
  return visited;
}

std::vector<std::set<ivector>> NavGrid::getConnectedComponents(){
  /* Returns a vector of vectors: each connected component of the NavGrid,
     each of which is a vector of integer coordinates
   */
  std::set<ivector> visited{};
  std::vector<std::set<ivector>> return_val = {};
  for(auto it = mNodes.begin(); it != mNodes.end(); it++){
    /* Check if node already visited */
    if(std::find(visited.begin(), visited.end(),  *it)!=visited.end()){
      continue;
    }
    /* Add node to visited, so we don't revisit then add this component */
    /* It doesn't matter whether we set include_diagonal to true or false */
    auto new_comp = getConnectedComponentFromNode(*it);
    for(auto node : new_comp)
      visited.insert(node);

    return_val.push_back(new_comp);
  }
  return return_val;
}

inline unsigned int manhatton_distance(ivector x, ivector y){
  return std::abs(x[0] - y[0]) + std::abs(x[1] - y[1]);
}

static std::vector<ivector> reconstruct_path(std::unordered_map<ivector, ivector, ivector_hash>& came_from, ivector current) {
  std::vector<ivector> total_path = { current };
  while (came_from.find(current) != came_from.end()) {
    current = came_from[current];
    total_path.push_back(current);
  }
  std::reverse(total_path.begin(), total_path.end());
  return total_path;
}

std::vector<ivector> NavGrid::findRoute(ivector start, ivector goal, bool include_diagonal){
  std::set<ivector> comp = getConnectedComponentFromNode(start);

  if(std::find(comp.begin(), comp.end(), goal) == comp.end()){
    /* No route to goal */
    return std::vector<ivector> {};
  }

  /* Use A* search algorithm */
  using pq_element = std::pair<int, ivector>;
  std::priority_queue<pq_element, std::vector<pq_element>, std::greater<pq_element>> open_set;

  open_set.emplace(0, start);

  std::unordered_map<ivector, ivector, ivector_hash> came_from;
  std::unordered_map<ivector, int, ivector_hash> g_score;
  g_score[start] = 0;

  std::unordered_map<ivector, int, ivector_hash> f_score;
  f_score[start] = manhatton_distance(start, goal);

  while(!open_set.empty()){
    ivector current = open_set.top().second;
    open_set.pop();

    if (current == goal) {
      /* We're done */
      return reconstruct_path(came_from, current);
    }

    for (const auto& neighbour : getNeighbours(current, include_diagonal=include_diagonal)) {
      int tentative_g_score = g_score[current] + 1;
      if (!g_score.count(neighbour) || tentative_g_score < g_score[neighbour]) {
        came_from[neighbour] = current;
        g_score[neighbour] = tentative_g_score;
        f_score[neighbour] = tentative_g_score + manhatton_distance(neighbour, goal);
        open_set.emplace(f_score[neighbour], neighbour);
      }
    }
  }
  /* Failed: return empty path */
  return std::vector<ivector>{};
}

void
NavGrid::addNode(ivector node)
{
  mNodes.insert(node);
}

void
NavGrid::removeNode(ivector node)
{
  if(!mNodes.contains(node))
    return;
  mNodes.erase(node);
}
