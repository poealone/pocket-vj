#include "input.h"
#include <cstdio>

bool Input::init() {
    // Open first available game controller
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            m_controller = SDL_GameControllerOpen(i);
            if (m_controller) {
                SDL_Log("Controller: %s", SDL_GameControllerName(m_controller));
                return true;
            }
        }
    }
    // No game controller found — keyboard only (desktop)
    SDL_Log("No game controller found, using keyboard");
    return true;
}

void Input::handleControllerButton(int sdlButton, bool down) {
    Button b = Button::COUNT;
    switch (sdlButton) {
        case SDL_CONTROLLER_BUTTON_A:             b = Button::A; break;
        case SDL_CONTROLLER_BUTTON_B:             b = Button::B; break;
        case SDL_CONTROLLER_BUTTON_X:             b = Button::X; break;
        case SDL_CONTROLLER_BUTTON_Y:             b = Button::Y; break;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  b = Button::L; break;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: b = Button::R; break;
        case SDL_CONTROLLER_BUTTON_START:         b = Button::START; break;
        case SDL_CONTROLLER_BUTTON_BACK:          b = Button::SELECT; break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:       b = Button::UP; break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:     b = Button::DOWN; break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:     b = Button::LEFT; break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:    b = Button::RIGHT; break;
        default: return;
    }

    if (b != Button::COUNT) {
        if (down) {
            if (!m_state.held[(int)b]) m_state.pressed[(int)b] = true;
            m_state.held[(int)b] = true;
        } else {
            m_state.released[(int)b] = true;
            m_state.held[(int)b] = false;
        }
    }
}

void Input::poll() {
    m_state.reset();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                m_state.quit = true;
                break;

            // Keyboard (desktop)
            case SDL_KEYDOWN: {
                if (e.key.repeat) break;
                Button b = keyToButton(e.key.keysym.sym);
                if (b != Button::COUNT) {
                    m_state.pressed[(int)b] = true;
                    m_state.held[(int)b] = true;
                }
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

            // Game controller buttons
            case SDL_CONTROLLERBUTTONDOWN:
                handleControllerButton(e.cbutton.button, true);
                break;
            case SDL_CONTROLLERBUTTONUP:
                handleControllerButton(e.cbutton.button, false);
                break;

            // Game controller axes (for L2/R2 triggers)
            case SDL_CONTROLLERAXISMOTION: {
                if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
                    bool active = e.caxis.value > 16000;
                    if (active && !m_state.held[(int)Button::L2]) m_state.pressed[(int)Button::L2] = true;
                    if (!active && m_state.held[(int)Button::L2]) m_state.released[(int)Button::L2] = true;
                    m_state.held[(int)Button::L2] = active;
                }
                if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
                    bool active = e.caxis.value > 16000;
                    if (active && !m_state.held[(int)Button::R2]) m_state.pressed[(int)Button::R2] = true;
                    if (!active && m_state.held[(int)Button::R2]) m_state.released[(int)Button::R2] = true;
                    m_state.held[(int)Button::R2] = active;
                }
                break;
            }

            // Handle controller connect/disconnect
            case SDL_CONTROLLERDEVICEADDED:
                if (!m_controller) {
                    m_controller = SDL_GameControllerOpen(e.cdevice.which);
                    if (m_controller) {
                        SDL_Log("Controller connected: %s", SDL_GameControllerName(m_controller));
                    }
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (m_controller && e.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_controller))) {
                    SDL_GameControllerClose(m_controller);
                    m_controller = nullptr;
                }
                break;
        }
    }
}

Button Input::keyToButton(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:     return Button::UP;
        case SDLK_DOWN:   return Button::DOWN;
        case SDLK_LEFT:   return Button::LEFT;
        case SDLK_RIGHT:  return Button::RIGHT;
        case SDLK_z:      return Button::A;
        case SDLK_x:      return Button::B;
        case SDLK_a:      return Button::X;
        case SDLK_s:      return Button::Y;
        case SDLK_q:      return Button::L;
        case SDLK_w:      return Button::R;
        case SDLK_e:      return Button::L2;
        case SDLK_r:      return Button::R2;
        case SDLK_RETURN: return Button::START;
        case SDLK_TAB:    return Button::SELECT;
        default:           return Button::COUNT;
    }
}
