#pragma once
#include <string>

namespace atlas::interaction {

class VoiceAdapter {
public:
    virtual ~VoiceAdapter() = default;
    virtual std::string Transcribe(const float* samples, int count) = 0;
};

} // namespace atlas::interaction
