#include "engine.hpp"
#include "Camera.hpp"

level :: level(double x, double y){
  mDimmension[0] = x;
  mDimmension[1] = y;
  // /*The SDL_TEXTUREACCESSTARGET flag allows us to modify this sprite*/
  // // mpSprite = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,1024, 1024);
  // SDL_SetRenderTarget(_renderer, mpSprite);
  // SDL_SetRenderDrawColor(_renderer, 0, 0xa0, 0xFF, 0xFF);
  // SDL_RenderClear(_renderer);
  // SDL_RenderFillRect(_renderer, NULL);
  // SDL_RenderPresent(_renderer);
  // SDL_SetRenderTarget(_renderer, NULL);
  
    return;
}

level :: ~level(){
  return;
}

void level :: ReloadSprites(){
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


void level::movementUpdate(){
    /*Iterate over all moving actors*/
    for (auto i = mActors.begin(); i != mActors.end(); i++) {
        /*Update actors*/
        (*i)->update();

        if ((*i)->is_moving())
            (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);
    }
    return;
}

void level::physicsUpdate() {
    /* Detect collisions */

    // TODO: Will be moving to region based collision checking eventually
    int iIndex = -1;
    int jIndex = -1;
    for (auto i = mActors.begin(); i != mActors.end(); i++) {
        iIndex++;
        if ((*i)->mCollides == false)
            continue;
        for (auto j = mActors.begin(); j != mActors.end(); j++) {
            jIndex++;
            // Iterate until distinct pair obtained
            if (jIndex <= iIndex || (*i) == (*j)) {
                continue;
            }

            if ((*j)->mCollides == false)
                continue;

            // Obtain separating axis
            dvector iAxesMtv = (*i)->testNormalAxes(*j);
            if (iAxesMtv[0] == 0 && iAxesMtv[1] == 0)
                continue;
            dvector jAxesMtv = (*i)->testNormalAxes(*j);
            if (jAxesMtv[0] == 0 && jAxesMtv[1] == 0)
                continue;

            // No separating axis found - find minimum translation vector (MTV)
            // MTV pointing away from i and j respectively
            dvector iMtv;
            dvector jMtv;

            // Determine which of the two candidate MTVs are minimum
            double iMtvSqrNorm = iAxesMtv[0] * iAxesMtv[0] +
                iAxesMtv[1] * iAxesMtv[1];
            double jMtvSqrNorm = jAxesMtv[0] * jAxesMtv[0] +
                jAxesMtv[1] * jAxesMtv[1];
            if (iMtvSqrNorm > jMtvSqrNorm) {
                jMtv = jAxesMtv;
                iMtv = {{-jMtv[0], -jMtv[1]}};
            } else {
                iMtv = iAxesMtv;
                jMtv = {{-iMtv[0], -iMtv[1]}};
            }

            // Register collision for both i and j
            (*i)->registerCollision(*j, iMtv);
            (*j)->registerCollision(*i, jMtv);
        }
    }

    // Apply collision updates
    for (auto i = mActors.begin(); i != mActors.end(); i++) {
        (*i)->resolveCollision();
    }
}

void level :: draw(Camera *cam){
  if(!cam){
    log_message(ERROR, "Attempted to draw with null camera object!");
    return;
  }

  SDL_SetRenderTarget(_renderer, cam->getFrameBuffer());

  SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
  SDL_RenderFillRect(_renderer, nullptr);

  //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw(cam);
  }

  /*  Draw all particles.*/
  for(auto i = mParticleList.begin(); i!= mParticleList.end(); i++){
    /* Remove the previous node if its remove flag is set */
    (*i)->draw(cam);
  }

 return;
}

/* TODO: move all update and movement code into this method  */
void level :: update(){
  mActors.remove_if([](std::shared_ptr<actor>a){return a->mRemove;});
  movementUpdate();
  physicsUpdate();

  for(auto i = mActors.begin(); i != mActors.end(); i++){
    (*i)->updateSprite();
  }

   return;
}
