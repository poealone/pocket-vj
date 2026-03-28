#pragma once
#include <SDL2/SDL.h>

// Button mapping for RG35XX / MUOS gamepad + keyboard fallback
enum class Button {
    UP, DOWN, LEFT, RIGHT,
    A, B, X, Y,
    L, R, L2, R2,
    START, SELECT,
    COUNT
};

struct InputState {
    bool pressed[(int)Button::COUNT]  = {};  // Just pressed this frame
    bool held[(int)Button::COUNT]     = {};  // Currently held
    bool released[(int)Button::COUNT] = {};  // Just released this frame
    bool quit = false;

    void reset() {
        for (int i = 0; i < (int)Button::COUNT; i++) {
            pressed[i] = false;
            released[i] = false;
        }
        quit = false;
    }
};

class Input {
public:
    void poll();
    const InputState& state() const { return m_state; }

    bool pressed(Button b)  const { return m_state.pressed[(int)b]; }
    bool held(Button b)     const { return m_state.held[(int)b]; }
    bool released(Button b) const { return m_state.released[(int)b]; }
    bool quit()             const { return m_state.quit; }

private:
    InputState m_state;

    // Keyboard mapping (desktop dev)
    Button keyToButton(SDL_Keycode key);
    // Gamepad mapping (MUOS/RG35XX)
    Button padToButton(uint8_t btn);
};
