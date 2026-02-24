#pragma once
#include "SoundGraph.h"

namespace atlas::sound {

// Generates sine wave audio buffer from frequency
class OscillatorNode : public SoundNode {
public:
    const char* GetName() const override { return "Oscillator"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<SoundPort> Inputs() const override;
    std::vector<SoundPort> Outputs() const override;
    void Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const override;
};

// Multiplies audio buffer by a gain float
class GainNode : public SoundNode {
public:
    const char* GetName() const override { return "Gain"; }
    const char* GetCategory() const override { return "Effect"; }
    std::vector<SoundPort> Inputs() const override;
    std::vector<SoundPort> Outputs() const override;
    void Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const override;
};

// Mixes two audio buffers together
class MixNode : public SoundNode {
public:
    const char* GetName() const override { return "Mix"; }
    const char* GetCategory() const override { return "Effect"; }
    std::vector<SoundPort> Inputs() const override;
    std::vector<SoundPort> Outputs() const override;
    void Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const override;
};

// Generates ADSR envelope from 4 float parameters
class EnvelopeNode : public SoundNode {
public:
    const char* GetName() const override { return "Envelope"; }
    const char* GetCategory() const override { return "Generator"; }
    std::vector<SoundPort> Inputs() const override;
    std::vector<SoundPort> Outputs() const override;
    void Evaluate(const SoundContext& ctx, const std::vector<SoundValue>& inputs, std::vector<SoundValue>& outputs) const override;
};

}
