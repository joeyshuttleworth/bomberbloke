#include "AbstractHudElement.hpp"
#include "Camera.hpp"
#include "CreationEvent.hpp"
#include "actor.hpp"
#include "engine.hpp"

extern std::list<std::shared_ptr<AbstractSpriteHandler>> _particle_list;

scene ::scene(double x, double y)
{
  mDimmension[0] = x;
  mDimmension[1] = y;
  mpCamera = std::make_shared<Camera>(this);
  return;
}

void
scene ::refreshSprites()
{
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    (*i)->refreshSprite();
  }

  for (auto i = _particle_list.begin(); i != _particle_list.end(); i++) {
    (*i)->refreshSprite();
  }

  SDL_SetRenderTarget(_renderer, NULL);
  return;
}

std::shared_ptr<actor>
scene ::GetActor(int id)
{
  /*search over actors*/
  auto iterator = std::find_if(
    mActors.begin(), mActors.end(), [&](std::shared_ptr<actor> a) -> bool {
      return a->getId() == id;
    });
  // return the first one (should be unique anyway)
  if (iterator != mActors.end())
    return *iterator;
  else
    return nullptr;
}

void
scene::cleanUp()
{
  /* Remove particles with mRemove set! */
  mParticleList.remove_if(
    [](std::shared_ptr<AbstractSpriteHandler> s) { return s->ToRemove(); });
  /* Now clean up actors */
  mActors.remove_if(
    [](std::shared_ptr<actor> a) -> bool { return a->toRemove(); });
  return;
}

void
scene::movementUpdate()
{
  /*Iterate over all moving actors*/
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    /*Update actors*/
    (*i)->update();
    (*i)->move((*i)->mPosition[0] + (*i)->mVelocity[0],
               (*i)->mPosition[1] + (*i)->mVelocity[1]);
  }
  return;
}

void
scene ::addActorWithId(std::shared_ptr<actor> a)
{
  /* Check the id hasn't been taken*/
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    if ((*i)->getId() == a->getId()) {
      log_message(ERR,
                  "Tried to add actor with id " + std::to_string(a->getId()) +
                    " but an actor with this id already exists!");
      return;
    }
  }

  /* Now add the actor to the back of the list */
  mActors.push_back(a);
}

void
scene ::addActor(std::shared_ptr<actor> a)
{
  for (int j = mLastActorId + 1; j - mLastActorId < 10000; j++) {
    bool set = true;
    for (auto i = mActors.begin(); i != mActors.end(); i++) {
      if ((*i)->getId() == j) {
        set = false;
        break;
      }
    }
    if (set) {
      mLastActorId = j;
      a->setId(j);
      mActors.push_back(a);
      if (_server) {
        /* Broadcast a EVENT_CREATE event */
        std::unique_ptr<AbstractEvent> c_event(new CreationEvent(a));
        _net_server.broadcastEvent(c_event);
      }
      return;
    }
  }
  log_message(ERR, "Failed to add actor - too many actors in mActors");
}

static bool
collides(AbstractCollider* a, AbstractCollider* b)
{
  dvector iAxesMtv = a->testNormalAxes(b);
  if (iAxesMtv[0] == 0 && iAxesMtv[1] == 0)
    return false;
  dvector jAxesMtv = a->testNormalAxes(b);
  if (jAxesMtv[0] == 0 && jAxesMtv[1] == 0)
    return false;
  else
    return true;
}

void
scene::physicsUpdate()
{
  /* Detect collisions */

  // TODO: Will be moving to region based collision checking eventually
  int iIndex = -1;
  int jIndex;
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    iIndex++;
    if ((*i)->mCollides == false)
      continue;

    jIndex = -1;
    for (auto j = mActors.begin(); j != mActors.end(); j++) {
      jIndex++;
      // Iterate until distinct pair obtained
      if (jIndex <= iIndex || (*i) == (*j))
        continue;

      if ((*j)->mCollides == false)
        continue;

      // Obtain separating axis
      dvector iAxesMtv = (*i)->testNormalAxes(*j);
      if (iAxesMtv[0] == 0 && iAxesMtv[1] == 0)
        continue;
      dvector jAxesMtv = (*j)->testNormalAxes(*i);
      if (jAxesMtv[0] == 0 && jAxesMtv[1] == 0)
        continue;
      // No separating axis found - find minimum translation vector (MTV)
      // MTV pointing away from i and j respectively
      dvector iMtv;
      dvector jMtv;

      // Determine which of the two candidate MTVs are minimum
      double iMtvSqrNorm =
        iAxesMtv[0] * iAxesMtv[0] + iAxesMtv[1] * iAxesMtv[1];
      double jMtvSqrNorm =
        jAxesMtv[0] * jAxesMtv[0] + jAxesMtv[1] * jAxesMtv[1];
      if (iMtvSqrNorm > jMtvSqrNorm) {
        jMtv = jAxesMtv;
        iMtv = { { -jMtv[0], -jMtv[1] } };
      } else {
        iMtv = iAxesMtv;
        jMtv = { { -iMtv[0], -iMtv[1] } };
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

void
scene::updateHudPositions()
{
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->updatePosition(mpCamera.get());
  }
}

void
scene::draw()
{
  mpCamera->resetFrameBuffer();

  drawActors();
  drawParticles();
  drawHud();

  mpCamera->draw();
}

void
scene::drawHud()
{
  if (!mpCamera) {
    log_message(ERR, "Attempted to draw with null camera object!");
    return;
  }

  // Draw HUD elements
  // SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->draw(mpCamera.get());
  }
}

void
scene::drawParticles()
{
  if (!mpCamera) {
    log_message(ERR, "Attempted to draw with null camera object!");
    return;
  }

  // SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());
  /*  Draw all particles.*/
  for (auto i = mParticleList.begin(); i != mParticleList.end(); i++) {
    (*i)->draw(mpCamera.get());
  }
}

void
scene::drawActors()
{
  if (!mpCamera) {
    log_message(ERR, "Attempted to draw with null camera object!");
    return;
  }

  // SDL_SetRenderTarget(_renderer, mpCamera->getFrameBuffer());
  // Next draw each actor
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    (*i)->draw(mpCamera.get());
  }
}

static void
interpolateActors(std::list<std::shared_ptr<actor>>& actors)
{
  for (auto i = actors.begin(); i != actors.end(); i++) {
    (*i)->interpolate();
  }
}

/* TODO: move all update and movement code into this method  */
void
scene ::update()
{
  if (!_server)
    interpolateActors(mActors);

  if (mpCamera)
    mpCamera->update();

  logicUpdate();

  if (mState == PLAYING)
    movementUpdate();

  cleanUp();
  physicsUpdate();
  updateSprites();
  return;
}

void
scene::updateSprites()
{
  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    (*i)->updateSprite();
  }

  for (auto i = mParticleList.begin(); i != mParticleList.end(); i++) {
    (*i)->update();
  }

  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    (*i)->update();
  }
}

std::list<std::shared_ptr<actor>>
scene::ActorsCollidingWith(AbstractCollider* p_collider)
{
  std::list<std::shared_ptr<actor>> r_list;

  for (auto i = mActors.begin(); i != mActors.end(); i++) {
    if (i->get() == p_collider)
      continue;
    else if (collides(p_collider, i->get())) {
      r_list.push_back(*i);
    }
  }

  return r_list;
}

void
scene::onInput(SDL_Event* event)
{
  // Let interactive HUD elements handle the detected input
  for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
    if ((*i)->mIsInteractive) {
      (*i)->onInput(event);
    }
  }
}

void
scene::onResize()
{
  std::lock_guard<std::mutex> guard{ mMutex };
  if (mpCamera)
    mpCamera->onResize();
  updateHudPositions();
  refreshSprites();
  return;
}

bool
scene::linkActorToPlayer(std::shared_ptr<actor>& act, unsigned int player_id)
{
  if (!act) {
    auto iter_player =
      std::find_if(_player_list.begin(),
                   _player_list.end(),
                   [&](std::shared_ptr<AbstractPlayer> p) -> bool {
                     return player_id == p->getId();
                   });
    (*iter_player)->setCharacter(nullptr);
    auto iter_l_player = std::find_if(
      _local_player_list.begin(),
      _local_player_list.end(),
      [&](LocalPlayer p) -> bool { return player_id == p.getId(); });
    iter_l_player->setCharacter(nullptr);
    return true;
  }

  bool linked = false;

  auto iter_actors =
    std::find_if(mActors.begin(),
                 mActors.end(),
                 [&](std::shared_ptr<actor> a) -> bool { return a == act; });

  if (iter_actors == mActors.end()) {
    log_message(ERR, "scene::linkActorToPlayer - actor was part of the scene!");
    return false;
  }

  act->setPlayerId(player_id);

  for (auto i = _player_list.begin(); i != _player_list.end(); i++) {
    if ((*i)->getId() == player_id) {
      (*i)->setCharacter(act);
      linked = true;
    } else if ((*i)->getCharacter() == act)
      (*i)->setCharacter(nullptr);
  }

  for (auto i = _local_player_list.begin(); i != _local_player_list.end();
       i++) {
    if (i->getId() == player_id) {
      std::cout << i->getId() << " " << player_id << "\n";
      i->setCharacter(act);
      linked = true;
    } else if (i->getCharacter() == act)
      i->setCharacter(nullptr);
  }

  return linked;
}
