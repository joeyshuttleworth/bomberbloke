#include "engine.hpp"
#include "Camera.hpp"

extern std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;

scene :: scene(double x, double y){
  mDimmension[0] = x;
  mDimmension[1] = y;
  return;
}

scene :: ~scene(){
  return;
}

void scene :: refreshSprites(){
  for(auto i = mActors.begin(); i != mActors.end(); i++){
    (*i)->refreshSprite();
  }

  for(auto i = _particle_list.begin(); i != _particle_list.end(); i++){
    (*i)->refreshSprite();
  }

  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

std::shared_ptr<actor> scene :: GetActor(int id){
  /*search over actors*/
  auto iterator = std::find_if(mActors.begin(), mActors.end(), [&](std::shared_ptr<actor> a) -> bool {return a->GetId() == id;});
  // return the first one (should be unique anyway)
  return *iterator;
}

void scene::cleanUp(){
  /* Remove particles with mRemove set! */
  mParticleList.remove_if([](std::shared_ptr<AbstractSpriteHandler> s){return s->ToRemove();});
  /* Now clean up actors */
  mActors.remove_if([](std::shared_ptr<actor> a){return a->mRemove;});
  return;
}


void scene::movementUpdate(){
    /*Iterate over all moving actors*/
    for (auto i = mActors.begin(); i != mActors.end(); i++) {
        /*Update actors*/
        (*i)->update();

        (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0], (*i)->mPosition[1] + (*i)->mVelocity[1]);
    }
    return;
}

void scene :: addActor(std::shared_ptr<actor> a){
  for(int j = mLastActorId + 1; j - mLastActorId < 10000; j++){
    bool set = true;
    for(auto i = mActors.begin(); i != mActors.end(); i++){
      if((*i)->GetId() == j){
        set = false;
        break;
      }
    }
    if(set){
      mLastActorId = j;
      a->setId(j);
      mActors.push_back(a);
      return;
    }
  }
  log_message(ERROR, "Failed to add actor - too many actors in mActors");
}

void scene::physicsUpdate() {
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

void scene :: draw(Camera *cam){
  // mpSpriteHandler->draw();
  int zoom = cam->GetZoom();

  SDL_SetRenderTarget(_renderer, cam->getFrameBuffer());

  SDL_SetRenderDrawColor(_renderer, 0x00, 0x10, 0xff, 0xff);
  SDL_RenderFillRect(_renderer, nullptr);

  SDL_SetRenderDrawColor(_renderer, 0x10, 0x10, 0x10, 0xa0);

  /* Draw a grid */
  for(int i = 0; i<=10; i++){
    SDL_RenderDrawLine(_renderer, i * zoom, 0, i * zoom, mDimmension[1]*zoom);
    SDL_RenderDrawLine(_renderer, 0, i*zoom, mDimmension[0]*zoom, i*zoom);
  }

  if(!cam){
    log_message(ERROR, "Attempted to draw with null camera object!");
    return;
  }

  //Next draw each actor
  for(auto i = mActors.begin(); i!=mActors.end(); i++){
    (*i)->draw(cam);
  }

  /*  Draw all particles.*/
  for(auto i = mParticleList.begin(); i!= mParticleList.end(); i++){
    (*i)->draw(cam);
  }

  return;
}

/* TODO: move all update and movement code into this method  */
void scene :: update(){
  mActors.remove_if([](std::shared_ptr<actor>a){return a->mRemove;});
  movementUpdate();
  physicsUpdate();
  updateSprites();
  return;
}

void scene::updateSprites(){
  for(auto i = mActors.begin(); i != mActors.end(); i++){
    (*i)->updateSprite();
  }
}
