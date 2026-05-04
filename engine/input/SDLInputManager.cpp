#include "SDLInputManager.hpp"
#include "scene.hpp"
#include "engine.hpp"


InputKey SDLInputManager::getPressedKey()
{

  switch(mEvent.getInputType()){
  case IEVENT_KEYDOWN:
  case IEVENT_KEYUP:
    break;
  default:
    return KEY_NONE;
  }

  switch(mEvent.key.keysym.scancode){
  case SDL_SCANCODE_ESCAPE:
    return KEY_ESC;
  case SDL_SCANCODE_RETURN:
    return KEY_RETURN
  case SDL_SCANCODE_BACKSPACE:
    return KEY_BACKSPACE;
  case SDL_SCANCODE_TAB:
    return KEY_TAB;
  case SDL_SCANCODE_RIGHT:
    return KEY_RIGHTARROW;
  case SDL_SCANCODE_LEFT:
    return KEY_LEFTARROW;
  case SDL_SCANCODE_UP:
    return KEY_UPARROW;
  case SDL_SCANCODE_DOWN:
    return KEY_DOWNARROW;
  }

  return KEY_NONE;
}


void SDLInputManager::handleInput(scene& scn){
   SDL_Event sdl_event;
  //  bool key_up = true;
  Uint8* kb_state = NULL;
  while (SDL_PollEvent(&sdl_event)) {

    switch (sdl_event.type) {
      case SDL_QUIT: {
        _halt = true;
        break;
      }
      case SDL_KEYDOWN: {
        if (!mBindNextKey)
          break;
        /*We only look at keyboard events here in order to bind keys*/
        CommandBinding new_binding;
        new_binding.scancode = sdl_event.key.keysym.scancode;
        new_binding.command = _next_bind_command;
        _local_player_list.back().mControlScheme.push_back(new_binding);
        mBindNextKey = false;
        log_message(INFO,
                    "Successfully bound " + new_binding.command + " to " +
                      std::to_string(new_binding.scancode));
        break;
      }
      case SDL_WINDOWEVENT: {
        if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
          scn.onResize();
          break;
        }

        default:{
          // Create an AbstractInputEvent derived event
          const SDLInputEvent event(sdl_event);
          scn.onInput(event);
        }
      }
    }
  }

  kb_state = (Uint8*)SDL_GetKeyboardState(NULL);

  /*Iterate over local players */
  for (auto i = _local_player_list.begin(); i != _local_player_list.end();
       i++) {
    /*Iterate over key bindings */
    if (sdl_event.type != SDL_JOYAXISMOTION) {
      for (auto j = i->mControlScheme.begin(); j != i->mControlScheme.end();
           j++) {
        if (kb_state[j->scancode] !=
            _kb_state[j->scancode]) { // ensure that current keymap is different
                                      // to old
          // prepend "+" or "-" to the command depending on keystate
          std::string command_to_send =
            kb_state[j->scancode] ? "+" + j->command : "-" + j->command;

          log_message(DEBUG, j->command);

          if (std::find(_system_commands.begin(),
                        _system_commands.end(),
                        split_to_tokens(j->command).front()) != _system_commands.end()) {
            handle_system_command(ctx,
              split_to_tokens(command_to_send)); // process system command
          } else {
            std::shared_ptr<actor> character = i->getCharacter();
            if (character) {
              character->handleCommand(command_to_send); // handle normal command

              if (!_server) {
                std::unique_ptr<AbstractEvent> c_event(
                  new CommandEvent(command_to_send));
                if(_net_client->mConnector)
                  _net_client->mConnector->broadcastEvent(std::move(c_event));
              }
            } else {
              log_message(
                DEBUG, "Input received but no character connected to player!");
            }
          }
        }
      }
    } else {
      if (i->getCharacter() && sdl_event.jaxis.which == 0) {
        if (sdl_event.jaxis.axis == 0) { // x axis
          if (sdl_event.jaxis.value < -DEADZONE) {
            i->getCharacter()->handleCommand("left" + dX);
          } else if (sdl_event.jaxis.value > DEADZONE) {
            i->getCharacter()->handleCommand("+right" + dX);
          } else {
            i->getCharacter()->handleCommand("-XAxis" + dX);
          }
        } else if (sdl_event.jaxis.axis == 1) {
          if (sdl_event.jaxis.value < -DEADZONE) {
            i->getCharacter()->handleCommand("+up" + dX);
          } else if (sdl_event.jaxis.value > DEADZONE) {
            i->getCharacter()->handleCommand("+down" + dX);
          } else {
            i->getCharacter()->handleCommand("-YAxis" + dX);
          }
        }
      }

    }
  }

  // old key state, new key state
  memcpy(_kb_state, kb_state, sizeof(Uint8) * SDL_SCANCODE_APP2);
  return;
}


SDLInputEvent::SDLInputEvent(SDL_Event event) : mSDLEvent(event)
{
  InputEventType e_type;

  switch(event.type){
  case SDL_QUIT:
    e_type = IEVENT_QUIT;
    break;
  case SDL_KEYDOWN:
    e_type = IEVENT_KEYDOWN;
    break;
  case SDL_KEYUP:
    e_type = IEVENT_KEYUP;
    break;
  case SDL_WINDOWEVENT:
    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
      e_type = IEVENT_WINDOWRESIZEEVENT;
    else
      e_type = IEVENT_NONE;
    break;
  case SDL_JOYAXISMOTION:
    e_type = IEVENT_JOYAXISMOTION;
    break;
  case SDL_MOUSEBUTTONUP:
    e_type = IEVENT_MOUSEBUTTONUP;
    break;
  case SDL_MOUSEBUTTONDOWN:
    e_type = IEVENT_MOUSEBUTTONDOWN;
    break;
  case SDL_MOUSEMOTION:
    e_type = IEVENT_MOUSEMOTION;
    break;
  default:
    e_type = IEVENT_NONE;
  }
  AbstractInputEvent(e_type);
}

SDLInputManager::init(){
  _kb_state = (Uint8*)malloc(sizeof(Uint8) * SDL_SCANCODE_APP2); // max scancode
  memset((void*)_kb_state, 0, sizeof(Uint8) * SDL_SCANCODE_APP2);
}


std::pair<std::string, unsigned int pos>
SDLInputManager::handleTextInput(std::string text, unsigned int pos,
                                 const AbstractInputEvent& _event)
{

  SDLInputEvent& sdl_input_event = _event;
  SDL_Event& event = sdl_input_event.mEvent;

  if (event.type == SDL_KEYDOWN) {
    const SDL_Keysym& key = event.key.keysym;
    bool ctrl = (key.mod & KMOD_CTRL);

    // Backspace (delete before cursor)
    if (key.sym == SDLK_BACKSPACE && pos > 0) {
      text = text.substr(0, pos - 1) +
                   text.substr(pos);
      pos--;
    }

    // Move cursor left
    else if (key.sym == SDLK_LEFT && pos > 0) {
      pos--;
    }

    // Move cursor right
    else if (key.sym == SDLK_RIGHT && pos < text.length()) {
      pos++;
    }

    // Paste (Ctrl+V)
    else if (key.sym == SDLK_v && ctrl) {
      char* clip = SDL_GetClipboardText();
      if (clip) {
        std::string pasteText(clip);
        SDL_free(clip);

        text = text.substr(0, pos) +
                     pasteText +
                     text.substr(pos);

        pos += pasteText.length();
      }
    }
  }

  else if (event.type == SDL_TEXTINPUT) {
    const char* input = event.text.text;

    text = text.substr(0, pos) +
                 input +
                 text.substr(pos);

    pos += SDL_strlen(input);
  }

  return {text, pos};
}
