#include "engine.hpp"

level :: level(){
  mDimmension[0] = 10;
  mDimmension[1] = 10;
  /*The SDL_TEXTUREACCESSTARGET flag allows us to modify this sprite*/
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);
  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

level :: ~level(){
  return;
}

level :: level(const level &lvl){
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
  memcpy(mpSprite, lvl.mpSprite, sizeof(SDL_Surface));
}

void level :: ReloadSprites(){
  if(mpSprite)
    SDL_DestroyTexture(mpSprite);
  mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  SDL_SetRenderTarget(_renderer, mpSprite);
  SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  SDL_RenderClear(_renderer);
  SDL_RenderFillRect(_renderer, NULL);
  SDL_RenderPresent(_renderer);

  for(auto i = mActors.begin(); i != mActors.end(); i++){
    (*i)->ReloadSprite();
  }

  SDL_SetRenderTarget(_renderer, NULL);
  return;
}
std::shared_ptr<actor> level :: GetActor(int id){
  /*search over actors*/
  auto iterator = std::find_if(mActors.begin(), mActors.end(), [&](std::shared_ptr<actor> a) -> bool {return a->GetId() == id;});
  // return the first one (should be unique anyway)
  return *iterator;
}

void level::cleanUp(){
  /* Remove particles with mRemove set! */
  mParticleList.remove_if([](std::shared_ptr<AbstractSpriteHandler> s){return s->ToRemove();});
  /* Now clean up actors */
  mActors.remove_if([](std::shared_ptr<actor> a){return a->mRemove;});
  return;
}


void level::handleMovement(){
  /*Iterate over all moving actors*/
    for (auto i = _level.mActors.begin(); i != _level.mActors.end(); i++) {
        bool collision = false;
        std::array<double,4> seperation_vectors;
        /*Update actors*/

        (*i)->update();

        if (!(*i)->is_moving())
            continue;
        if ((*i)->mCollides == false)
            (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);

        else {
            /*Check for collisions*/
            for (auto j = _level.mActors.begin(); j != _level.mActors.end(); j++) {
                if (i == j)
                    continue;
                if ((*j)->mCollides == true){
                  seperation_vectors = detectCollision(*i, *j);
                  unsigned int non_zeros = 0;
                  for(unsigned int i = 0; i < 4; i++){
                    if(seperation_vectors[i]!=0)
                      non_zeros++;
                  }
                  if(non_zeros>0){
                    collision = true;
                    break;
                  }
                }
                if (collision)
                  break;
            }
        }
        if (!collision)
          (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);
        else{
          /* TODO apply seperation vectors to the actors */
        }
    }
    return;
}

void level :: draw(){
  // SDL_Rect rect;
  // int rgb[3];

  SDL_SetRenderTarget(_renderer, NULL);

  if(!mpSprite){
    mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 1024);
  }

  SDL_RenderCopy(_renderer, mpSprite, NULL, NULL);

    //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw();
  }
  /*  Draw all particles.*/
  for(auto i = mParticleList.begin(); i!= mParticleList.end(); i++){
    /* Remove the previous node if its remove flag is set */
    (*i)->draw();
  }

 return;
}


 std::array<double,4> level::detectCollision(std::shared_ptr<actor> a, std::shared_ptr<actor> b){
    bool collision = true;
    std::array<double, 4> seperation_vector = {0,0,0,0};
    std::vector<double> aPosition = {a->mPosition[0], a->mPosition[1]};
    std::vector<double> bPosition = {b->mPosition[0], b->mPosition[1]};

    // Test axes normal to actor a
    // TODO: Move test axis generation into ColliderFrame
    int aNVertices =  a->mColliderFrame.mFrameVertices.size();
    for (int i = 0; i < aNVertices - 1; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::vector<double> axis = {
            a->mColliderFrame.mFrameVertices[i][1] - a->mColliderFrame.mFrameVertices[i + 1][1],
            a->mColliderFrame.mFrameVertices[i + 1][0] - a->mColliderFrame.mFrameVertices[i][0]
        };

        std::pair<double, double> aProjVal = a->mColliderFrame.projectOntoAxis(axis, aPosition);
        std::pair<double, double> bProjVal = b->mColliderFrame.projectOntoAxis(axis, bPosition);

        if (aProjVal.second < bProjVal.first || aProjVal.first > bProjVal.second)
            collision = false;
    }

    // Test axes normal to actor b
    int bNVertices =  b->mColliderFrame.mFrameVertices.size();
    for (int i = 0; i < bNVertices - 1; i++) {
        // Normal axis is given (-y, x) where (x, y) = v2 - v1
        std::vector<double> axis = {
            b->mColliderFrame.mFrameVertices[i][1] - a->mColliderFrame.mFrameVertices[i + 1][1],
            b->mColliderFrame.mFrameVertices[i + 1][0] - a->mColliderFrame.mFrameVertices[i][0]
        };

        std::pair<double, double> aProjVal = a->mColliderFrame.projectOntoAxis(axis, aPosition);
        std::pair<double, double> bProjVal = b->mColliderFrame.projectOntoAxis(axis, bPosition);

        if (aProjVal.second < bProjVal.first || aProjVal.first > bProjVal.second){
            collision = false;
        }
    }
    /*  TODO: compute seperation vectors */
    if(collision){
      seperation_vector[0]=1;
    }
    return seperation_vector;
}
