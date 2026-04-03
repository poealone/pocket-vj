#include "music_player.h"
#include <SDL2/SDL.h>
#include <algorithm>

bool MusicPlayer::init() {
    if (m_initialized) return true;
#ifdef HAVE_SDL_MIXER
    int flags   = MIX_INIT_OGG | MIX_INIT_MP3;
    int initted = Mix_Init(flags);
    if (!(initted & (MIX_INIT_OGG | MIX_INIT_MP3))) {
        SDL_Log("MusicPlayer: Mix_Init failed: %s", Mix_GetError());
        // Not fatal — may still work for WAV
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("MusicPlayer: Mix_OpenAudio failed: %s", Mix_GetError());
        Mix_Quit();
        return false;
    }
    Mix_VolumeMusic(m_volume);
#endif
    m_initialized = true;
    return true;
}

void MusicPlayer::shutdown() {
    if (!m_initialized) return;
#ifdef HAVE_SDL_MIXER
    if (m_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
    Mix_CloseAudio();
    Mix_Quit();
#endif
    m_path.clear();
    m_initialized = false;
}

bool MusicPlayer::load(const std::string& path) {
#ifdef HAVE_SDL_MIXER
    if (!m_initialized) return false;
    if (m_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
    m_path.clear();
    m_music = Mix_LoadMUS(path.c_str());
    if (!m_music) {
        SDL_Log("MusicPlayer: Mix_LoadMUS failed (%s): %s", path.c_str(), Mix_GetError());
        return false;
    }
    m_path = path;
    return true;
#else
    (void)path;
    return false;
#endif
}

void MusicPlayer::play() {
#ifdef HAVE_SDL_MIXER
    if (!m_music) return;
    Mix_PlayMusic(m_music, -1);
#endif
}

void MusicPlayer::pause() {
#ifdef HAVE_SDL_MIXER
    if (Mix_PlayingMusic()) Mix_PauseMusic();
#endif
}

void MusicPlayer::resume() {
#ifdef HAVE_SDL_MIXER
    if (Mix_PausedMusic()) Mix_ResumeMusic();
#endif
}

void MusicPlayer::stop() {
#ifdef HAVE_SDL_MIXER
    Mix_HaltMusic();
    if (m_music) {
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
#endif
    m_path.clear();
}

void MusicPlayer::togglePause() {
    if (isPlaying())     pause();
    else if (isPaused()) resume();
}

bool MusicPlayer::isLoaded() const {
#ifdef HAVE_SDL_MIXER
    return m_music != nullptr;
#else
    return false;
#endif
}

bool MusicPlayer::isPlaying() const {
#ifdef HAVE_SDL_MIXER
    return Mix_PlayingMusic() != 0 && Mix_PausedMusic() == 0;
#else
    return false;
#endif
}

bool MusicPlayer::isPaused() const {
#ifdef HAVE_SDL_MIXER
    return Mix_PausedMusic() != 0;
#else
    return false;
#endif
}

void MusicPlayer::setVolume(int v) {
    m_volume = std::max(0, std::min(128, v));
#ifdef HAVE_SDL_MIXER
    Mix_VolumeMusic(m_volume);
#endif
}

std::string MusicPlayer::currentTitle() const {
    if (m_path.empty()) return "";
    size_t slash = m_path.rfind('/');
    std::string name = (slash != std::string::npos) ? m_path.substr(slash + 1) : m_path;
    size_t dot = name.rfind('.');
    if (dot != std::string::npos) name = name.substr(0, dot);
    return name;
}
