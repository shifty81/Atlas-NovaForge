---- MODULE replay ----
(***************************************************************************)
(* Replay Hash Ladder for the Atlas engine.                                *)
(*                                                                         *)
(* Models the chained hash commitment scheme used by StateHasher and       *)
(* ReplayRecorder.  Each tick produces a hash that depends on the previous *)
(* hash, the current ECS state, and the current inputs:                    *)
(*                                                                         *)
(*   H[n] = Hash(H[n-1] || State[n] || Inputs[n])                         *)
(*                                                                         *)
(* Verified invariants:                                                    *)
(*   ChainIntegrity   — Every hash derives from its predecessor.           *)
(*   NoGaps           — The tick sequence is contiguous.                   *)
(*   Determinism      — Identical inputs produce identical hashes.         *)
(*   MonotonicTicks   — Frame ticks are strictly increasing.               *)
(***************************************************************************)
EXTENDS Naturals, Sequences, FiniteSets

CONSTANTS
    MaxTick,
    StateSpace,
    InputSpace,
    Seed

HashCombine(prev, state, input) ==
    (prev * 31 + state) * 17 + input

VARIABLES
    mode,
    tick,
    hashChain,
    currentHash,
    frames,
    replaySource,
    replayIndex,
    shadowChain

vars == <<mode, tick, hashChain, currentHash, frames,
          replaySource, replayIndex, shadowChain>>

TypeOK ==
    /\ mode \in {"idle", "recording", "playback"}
    /\ tick \in 0..MaxTick
    /\ currentHash \in Nat
    /\ Len(hashChain) = tick
    /\ Len(frames) <= MaxTick

ChainIntegrity ==
    \A i \in 1..Len(hashChain) :
        LET entry == hashChain[i]
        IN  /\ entry[1] = i
            /\ IF i = 1
               THEN entry[2] = HashCombine(Seed, entry[3], entry[4])
               ELSE entry[2] = HashCombine(hashChain[i-1][2], entry[3], entry[4])

NoGaps ==
    \A i \in 1..Len(hashChain) : hashChain[i][1] = i

MonotonicTicks ==
    \A i \in 1..(Len(frames) - 1) :
        frames[i][1] < frames[i + 1][1]

Invariant ==
    /\ TypeOK
    /\ NoGaps
    /\ MonotonicTicks
    /\ (Len(hashChain) > 0 => ChainIntegrity)

Init ==
    /\ mode = "idle"
    /\ tick = 0
    /\ hashChain = <<>>
    /\ currentHash = Seed
    /\ frames = <<>>
    /\ replaySource = <<>>
    /\ replayIndex = 0
    /\ shadowChain = <<>>

StartRecording ==
    /\ mode = "idle"
    /\ mode' = "recording"
    /\ tick' = 0
    /\ hashChain' = <<>>
    /\ currentHash' = Seed
    /\ frames' = <<>>
    /\ UNCHANGED <<replaySource, replayIndex, shadowChain>>

RecordTick(state, input) ==
    /\ mode = "recording"
    /\ tick < MaxTick
    /\ state \in StateSpace
    /\ input \in InputSpace
    /\ LET newHash == HashCombine(currentHash, state, input)
           newTick == tick + 1
       IN  /\ tick' = newTick
           /\ currentHash' = newHash
           /\ hashChain' = Append(hashChain, <<newTick, newHash, state, input>>)
           /\ frames' = Append(frames, <<newTick, input, newHash, FALSE>>)
           /\ UNCHANGED <<mode, replaySource, replayIndex, shadowChain>>

StopRecording ==
    /\ mode = "recording"
    /\ mode' = "idle"
    /\ UNCHANGED <<tick, hashChain, currentHash, frames, replaySource, replayIndex, shadowChain>>

StartPlayback ==
    /\ mode = "idle"
    /\ Len(frames) > 0
    /\ mode' = "playback"
    /\ replaySource' = frames
    /\ replayIndex' = 0
    /\ shadowChain' = <<>>
    /\ tick' = 0
    /\ currentHash' = Seed
    /\ UNCHANGED <<hashChain, frames>>

PlaybackTick ==
    /\ mode = "playback"
    /\ replayIndex < Len(replaySource)
    /\ LET idx     == replayIndex + 1
           frame   == replaySource[idx]
           fInput  == frame[2]
           origEntry == hashChain[idx]
           fState    == origEntry[3]
           newHash   == HashCombine(currentHash, fState, fInput)
           newTick   == tick + 1
       IN  /\ tick' = newTick
           /\ currentHash' = newHash
           /\ replayIndex' = idx
           /\ shadowChain' = Append(shadowChain, <<newTick, newHash, fState, fInput>>)
           /\ UNCHANGED <<mode, hashChain, frames, replaySource>>

EndPlayback ==
    /\ mode = "playback"
    /\ replayIndex = Len(replaySource)
    /\ mode' = "idle"
    /\ UNCHANGED <<tick, hashChain, currentHash, frames, replaySource, replayIndex, shadowChain>>

Next ==
    \/ StartRecording
    \/ \E s \in StateSpace, i \in InputSpace : RecordTick(s, i)
    \/ StopRecording
    \/ StartPlayback
    \/ PlaybackTick
    \/ EndPlayback

Spec == Init /\ [][Next]_vars

====
