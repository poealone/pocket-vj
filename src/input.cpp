#include "input.h"

void Input::poll() {
    m_state.reset();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                m_state.quit = true;
                break;

            case SDL_KEYDOWN: {
                if (e.key.repeat) break;
                Button b = keyToButton(e.key.keysym.sym);
                if (b != Button::COUNT) {
                    m_state.pressed[(int)b] = true;
                    m_state.held[(int)b] = true;
                }
                // Escape = quit on desktop
                if (e.key.keysym.sym == SDLK_ESCAPE) m_state.quit = true;
                break;
            }

            case SDL_KEYUP: {
                Button b = keyToButton(e.key.keysym.sym);
                if (b != Button::COUNT) {
                    m_state.released[(int)b] = true;
                    m_state.held[(int)b] = false;
                }
                break;
            }

            // Gamepad (MUOS / RG35XX)
            case SDL_JOYBUTTONDOWN: {
                Button b = padToButton(e.jbutton.button);
                if (b != Button::COUNT) {
                    m_state.pressed[(int)b] = true;
                    m_state.held[(int)b] = true;
                }
                break;
            }

            case SDL_JOYBUTTONUP: {
                Button b = padToButton(e.jbutton.button);
                if (b != Button::COUNT) {
                    m_state.released[(int)b] = true;
                    m_state.held[(int)b] = false;
                }
                break;
            }

            case SDL_JOYHATMOTION: {
                // D-pad via hat
                m_state.held[(int)Button::UP]    = (e.jhat.value & SDL_HAT_UP);
                m_state.held[(int)Button::DOWN]  = (e.jhat.value & SDL_HAT_DOWN);
                m_state.held[(int)Button::LEFT]  = (e.jhat.value & SDL_HAT_LEFT);
                m_state.held[(int)Button::RIGHT] = (e.jhat.value & SDL_HAT_RIGHT);
                break;
            }
        }
    }
}

Button Input::keyToButton(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:     return Button::UP;
        case SDLK_DOWN:   return Button::DOWN;
        case SDLK_LEFT:   return Button::LEFT;
        case SDLK_RIGHT:  return Button::RIGHT;
        case SDLK_z:      return Button::A;       // A button
        case SDLK_x:      return Button::B;       // B button
        case SDLK_a:      return Button::X;       // X button
        case SDLK_s:      return Button::Y;       // Y button
        case SDLK_q:      return Button::L;       // L shoulder
        case SDLK_w:      return Button::R;       // R shoulder
        case SDLK_e:      return Button::L2;      // L2
        case SDLK_r:      return Button::R2;      // R2
        case SDLK_RETURN: return Button::START;
        case SDLK_TAB:    return Button::SELECT;
        default:           return Button::COUNT;
    }
}

Button Input::padToButton(uint8_t btn) {
    // RG35XX MUOS button mapping (adjust if needed)
    switch (btn) {
        case 0:  return Button::A;
        case 1:  return Button::B;
        case 2:  return Button::X;
        case 3:  return Button::Y;
        case 4:  return Button::L;
        case 5:  return Button::R;
        case 6:  return Button::L2;
        case 7:  return Button::R2;
        case 8:  return Button::SELECT;
        case 9:  return Button::START;
        default: return Button::COUNT;
    }
}
