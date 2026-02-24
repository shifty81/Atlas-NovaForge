#include "SoundNodes.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace atlas::sound {

// --- OscillatorNode ---

std::vector<SoundPort> OscillatorNode::Inputs() const {
    return {{"Frequency", SoundPinType::Float}};
}

std::vector<SoundPort> OscillatorNode::Outputs() const {
    return {{"Audio", SoundPinType::AudioBuffer}};
}

void OscillatorNode::Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const {
    float freq = 440.0f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        freq = inputs[0].data[0];
    }

    outputs.resize(1);
    outputs[0].type = SoundPinType::AudioBuffer;
    outputs[0].data.resize(ctx.bufferSize);

    double phaseInc = 2.0 * M_PI * static_cast<double>(freq) / static_cast<double>(ctx.sampleRate);
    double phase = 0.0;
    for (uint32_t i = 0; i < ctx.bufferSize; ++i) {
        outputs[0].data[i] = static_cast<float>(std::sin(phase));
        phase += phaseInc;
    }
}

// --- GainNode ---

std::vector<SoundPort> GainNode::Inputs() const {
    return {
        {"Audio", SoundPinType::AudioBuffer},
        {"Gain", SoundPinType::Float}
    };
}

std::vector<SoundPort> GainNode::Outputs() const {
    return {{"Audio", SoundPinType::AudioBuffer}};
}

void GainNode::Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const {
    float gain = 1.0f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        gain = inputs[1].data[0];
    }

    outputs.resize(1);
    outputs[0].type = SoundPinType::AudioBuffer;
    outputs[0].data.resize(ctx.bufferSize, 0.0f);

    if (!inputs.empty() && !inputs[0].data.empty()) {
        for (uint32_t i = 0; i < ctx.bufferSize && i < inputs[0].data.size(); ++i) {
            outputs[0].data[i] = inputs[0].data[i] * gain;
        }
    }
}

// --- MixNode ---

std::vector<SoundPort> MixNode::Inputs() const {
    return {
        {"AudioA", SoundPinType::AudioBuffer},
        {"AudioB", SoundPinType::AudioBuffer}
    };
}

std::vector<SoundPort> MixNode::Outputs() const {
    return {{"Audio", SoundPinType::AudioBuffer}};
}

void MixNode::Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = SoundPinType::AudioBuffer;
    outputs[0].data.resize(ctx.bufferSize, 0.0f);

    for (uint32_t i = 0; i < ctx.bufferSize; ++i) {
        float a = 0.0f;
        float b = 0.0f;
        if (!inputs.empty() && i < inputs[0].data.size()) a = inputs[0].data[i];
        if (inputs.size() > 1 && i < inputs[1].data.size()) b = inputs[1].data[i];
        outputs[0].data[i] = (a + b) * 0.5f;
    }
}

// --- EnvelopeNode ---

std::vector<SoundPort> EnvelopeNode::Inputs() const {
    return {
        {"Attack", SoundPinType::Float},
        {"Decay", SoundPinType::Float},
        {"Sustain", SoundPinType::Float},
        {"Release", SoundPinType::Float}
    };
}

std::vector<SoundPort> EnvelopeNode::Outputs() const {
    return {{"Envelope", SoundPinType::Envelope}};
}

void EnvelopeNode::Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const {
    float attack  = (inputs.size() > 0 && !inputs[0].data.empty()) ? inputs[0].data[0] : 0.01f;
    float decay   = (inputs.size() > 1 && !inputs[1].data.empty()) ? inputs[1].data[0] : 0.1f;
    float sustain = (inputs.size() > 2 && !inputs[2].data.empty()) ? inputs[2].data[0] : 0.7f;
    float release = (inputs.size() > 3 && !inputs[3].data.empty()) ? inputs[3].data[0] : 0.2f;

    float sr = static_cast<float>(ctx.sampleRate);
    uint32_t attackSamples  = static_cast<uint32_t>(attack * sr);
    uint32_t decaySamples   = static_cast<uint32_t>(decay * sr);
    uint32_t releaseSamples = static_cast<uint32_t>(release * sr);

    outputs.resize(1);
    outputs[0].type = SoundPinType::Envelope;
    outputs[0].data.resize(ctx.bufferSize);

    for (uint32_t i = 0; i < ctx.bufferSize; ++i) {
        float val;
        if (i < attackSamples) {
            val = static_cast<float>(i) / static_cast<float>(attackSamples);
        } else if (i < attackSamples + decaySamples) {
            float t = static_cast<float>(i - attackSamples) / static_cast<float>(decaySamples);
            val = 1.0f - t * (1.0f - sustain);
        } else if (i < ctx.bufferSize - releaseSamples) {
            val = sustain;
        } else {
            float t = static_cast<float>(i - (ctx.bufferSize - releaseSamples)) / static_cast<float>(releaseSamples);
            val = sustain * (1.0f - t);
        }
        outputs[0].data[i] = std::max(0.0f, std::min(1.0f, val));
    }
}

}
