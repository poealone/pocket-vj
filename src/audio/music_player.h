#pragma once
#include <string>

// Conditionally include SDL_mixer — available in ARM sysroot, optional on desktop
#if __has_include(<SDL2/SDL_mixer.h>)
#define HAVE_SDL_MIXER 1
#include <SDL2/SDL_mixer.h>
#endif

class MusicPlayer {
public:
    bool init();        // Mix_OpenAudio + Mix_Init(OGG|MP3)
    void shutdown();    // Mix_FreeMusic, Mix_CloseAudio, Mix_Quit

    bool load(const std::string& path);  // Mix_LoadMUS
    void play();         // Mix_PlayMusic(-1 = loop)
    void pause();        // Mix_PauseMusic
    void resume();       // Mix_ResumeMusic
    void stop();         // Mix_HaltMusic + free
    void togglePause();

    bool isLoaded()  const;
    bool isPlaying() const;  // Mix_PlayingMusic && !Mix_PausedMusic
    bool isPaused()  const;  // Mix_PausedMusic

    int  volume()    const { return m_volume; }
    void setVolume(int v);            // clamped 0-128
    void volumeUp()   { setVolume(m_volume + 8); }
    void volumeDown() { setVolume(m_volume - 8); }

    const std::string& currentPath()  const { return m_path; }
    std::string        currentTitle() const; // filename without extension

private:
#ifdef HAVE_SDL_MIXER
    Mix_Music* m_music = nullptr;
#endif
    std::string m_path;
    int  m_volume      = 80;
    bool m_initialized = false;
};
