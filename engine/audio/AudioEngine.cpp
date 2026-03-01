#include "AudioEngine.h"
#include <algorithm>
#include <cmath>

namespace atlas::audio {

void AudioEngine::Init() {
    m_sounds.clear();
    m_nextId = 1;
    m_masterVolume = 1.0f;
    m_initialized = true;
}

void AudioEngine::Shutdown() {
    for (auto& [id, source] : m_sounds) {
        source.state = SoundState::Stopped;
    }
    m_sounds.clear();
    m_initialized = false;
}

SoundID AudioEngine::LoadSound(const std::string& name) {
    SoundID id = m_nextId++;
    SoundSource source;
    source.id = id;
    source.name = name;
    m_sounds[id] = source;
    return id;
}

void AudioEngine::UnloadSound(SoundID id) {
    m_sounds.erase(id);
}

bool AudioEngine::HasSound(SoundID id) const {
    return m_sounds.count(id) > 0;
}

size_t AudioEngine::SoundCount() const {
    return m_sounds.size();
}

void AudioEngine::Play(SoundID id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.state = SoundState::Playing;
    }
}

void AudioEngine::Pause(SoundID id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end() && it->second.state == SoundState::Playing) {
        it->second.state = SoundState::Paused;
    }
}

void AudioEngine::Stop(SoundID id) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.state = SoundState::Stopped;
    }
}

SoundState AudioEngine::GetState(SoundID id) const {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        return it->second.state;
    }
    return SoundState::Stopped;
}

void AudioEngine::SetVolume(SoundID id, float volume) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.volume = std::clamp(volume, 0.0f, 1.0f);
    }
}

float AudioEngine::GetVolume(SoundID id) const {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        return it->second.volume;
    }
    return 0.0f;
}

void AudioEngine::SetPitch(SoundID id, float pitch) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.pitch = std::max(0.1f, pitch);
    }
}

float AudioEngine::GetPitch(SoundID id) const {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        return it->second.pitch;
    }
    return 1.0f;
}

void AudioEngine::SetLooping(SoundID id, bool looping) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.looping = looping;
    }
}

bool AudioEngine::IsLooping(SoundID id) const {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        return it->second.looping;
    }
    return false;
}

void AudioEngine::SetPosition(SoundID id, float x, float y, float z) {
    auto it = m_sounds.find(id);
    if (it != m_sounds.end()) {
        it->second.posX = x;
        it->second.posY = y;
        it->second.posZ = z;
    }
}

void AudioEngine::SetMasterVolume(float volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

float AudioEngine::GetMasterVolume() const {
    return m_masterVolume;
}

void AudioEngine::SetListenerPosition(float x, float y, float z) {
    m_listenerX = x;
    m_listenerY = y;
    m_listenerZ = z;
}

float AudioEngine::EffectiveVolume(SoundID id) const {
    auto it = m_sounds.find(id);
    if (it == m_sounds.end()) {
        return 0.0f;
    }
    const auto& src = it->second;
    if (src.state != SoundState::Playing) {
        return 0.0f;
    }

    float dx = src.posX - m_listenerX;
    float dy = src.posY - m_listenerY;
    float dz = src.posZ - m_listenerZ;
    float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

    float attenuation = 1.0f;
    if (m_maxDistance > 0.0f && dist > 0.0f) {
        attenuation = std::max(0.0f, 1.0f - dist / m_maxDistance);
    }

    return m_masterVolume * src.volume * attenuation;
}

void AudioEngine::SetMaxDistance(float dist) {
    m_maxDistance = std::max(0.0f, dist);
}

float AudioEngine::GetMaxDistance() const {
    return m_maxDistance;
}

void AudioEngine::Update(float dt) {
    (void)dt;
    // Stop non-looping sounds that have finished.
    // In a real engine with audio buffers this would check playback
    // position; here we leave state management to Play/Stop/Pause.

    // Compute effective volumes for active sources so higher-level
    // systems (e.g. a mixer or diagnostics panel) can query them.
    // The per-source effective volume accounts for master volume
    // and distance-based attenuation from the listener position.
    // Callers can use EffectiveVolume(id) after Update().
}

}
