#include "menu.h"

void Menu::open() { m_open = true; m_cursor = 0; }
void Menu::close() { m_open = false; }

void Menu::update(Input& input) {
    if (!m_open) return;

    if (input.pressed(Button::DOWN)) {
        m_cursor = (m_cursor + 1) % (int)items.size();
    }
    if (input.pressed(Button::UP)) {
        m_cursor = (m_cursor - 1 + (int)items.size()) % (int)items.size();
    }
    if (input.pressed(Button::B)) {
        close();
    }
}

void Menu::render(Renderer& r) {
    if (!m_open) return;

    // Semi-transparent overlay
    int mw = 200;
    int mh = 34 + (int)items.size() * 14 + 20;
    int mx = (RENDER_W - mw) / 2;
    int my = (RENDER_H - mh) / 2;
    r.rect(mx, my, mw, mh, {10, 10, 16}, true);
    r.rect(mx, my, mw, mh, Palette::UI_FG, false);

    r.textCentered(my + 8, APP_NAME, Palette::RED, 2);
    r.line(mx + 10, my + 26, mx + mw - 10, my + 26, Palette::GRID);

    for (int i = 0; i < (int)items.size(); i++) {
        int iy = my + 34 + i * 14;
        if (i == m_cursor) {
            r.rect(mx + 4, iy - 2, mw - 8, 12, {40, 20, 30}, true);
            r.text(mx + 12, iy, "> " + items[i], Palette::RED);
        } else {
            r.text(mx + 18, iy, items[i], Palette::UI_FG);
        }
    }

    r.text(mx + 10, my + mh - 14, "A:SELECT  B:BACK", Palette::GRID);
}
