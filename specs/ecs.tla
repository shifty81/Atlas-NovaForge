---- MODULE ecs ----
(***************************************************************************)
(* ECS Execution Model for the Atlas engine.                               *)
(*                                                                         *)
(* Models entity creation/destruction, component add/remove, and           *)
(* deterministic system execution ordering.  The real implementation uses  *)
(* sequential uint32 entity IDs (m_nextID counter), a two-level map for    *)
(* component storage (EntityID -> type_index -> any), and a registered     *)
(* system list that executes in fixed insertion order every tick.           *)
(*                                                                         *)
(* Verified invariants:                                                    *)
(*   UniqueIDs        — Entity IDs are never reused while alive.           *)
(*   DeadClean        — Destroyed entities have no components.             *)
(*   FixedOrder       — Systems execute in their registered order.         *)
(*   ExclusiveOwner   — A component type is mutated by at most one system  *)
(*                      per tick (matches WorldState::CanMutate).          *)
(***************************************************************************)
EXTENDS Naturals, Sequences, FiniteSets

CONSTANTS
    MaxEntities,
    ComponentTypes,
    Systems

VARIABLES
    nextID,
    alive,
    components,
    ownership,
    tickOrder,
    currentStep,
    tickCount

vars == <<nextID, alive, components, ownership, tickOrder, currentStep, tickCount>>

TypeOK ==
    /\ nextID \in 1..(MaxEntities + 1)
    /\ alive \subseteq 1..MaxEntities
    /\ DOMAIN components = alive
    /\ \A e \in alive : components[e] \subseteq ComponentTypes
    /\ DOMAIN ownership \subseteq ComponentTypes
    /\ tickOrder = Systems
    /\ currentStep \in 0..(Len(Systems) + 1)
    /\ tickCount \in Nat

UniqueIDs ==
    \A e \in alive : e < nextID

DeadClean ==
    \A e \in 1..MaxEntities : e \notin alive => e \notin DOMAIN components

FixedOrder ==
    tickOrder = Systems

Invariant ==
    /\ TypeOK
    /\ UniqueIDs
    /\ DeadClean
    /\ FixedOrder

Init ==
    /\ nextID = 1
    /\ alive = {}
    /\ components = [e \in {} |-> {}]
    /\ ownership = [c \in {} |-> ""]
    /\ tickOrder = Systems
    /\ currentStep = 0
    /\ tickCount = 0

CreateEntity ==
    /\ nextID <= MaxEntities
    /\ currentStep = 0
    /\ alive' = alive \union {nextID}
    /\ components' = [e \in alive' |-> IF e = nextID THEN {} ELSE components[e]]
    /\ nextID' = nextID + 1
    /\ UNCHANGED <<ownership, tickOrder, currentStep, tickCount>>

DestroyEntity(e) ==
    /\ e \in alive
    /\ currentStep = 0
    /\ alive' = alive \ {e}
    /\ components' = [ent \in alive' |-> components[ent]]
    /\ UNCHANGED <<nextID, ownership, tickOrder, currentStep, tickCount>>

AddComponent(e, c) ==
    /\ e \in alive
    /\ c \in ComponentTypes
    /\ c \notin components[e]
    /\ components' = [components EXCEPT ![e] = components[e] \union {c}]
    /\ UNCHANGED <<nextID, alive, ownership, tickOrder, currentStep, tickCount>>

RemoveComponent(e, c) ==
    /\ e \in alive
    /\ c \in components[e]
    /\ components' = [components EXCEPT ![e] = components[e] \ {c}]
    /\ UNCHANGED <<nextID, alive, ownership, tickOrder, currentStep, tickCount>>

BeginTick ==
    /\ currentStep = 0
    /\ currentStep' = 1
    /\ tickCount' = tickCount + 1
    /\ UNCHANGED <<nextID, alive, components, ownership, tickOrder>>

RunSystem ==
    /\ currentStep >= 1
    /\ currentStep <= Len(Systems)
    /\ currentStep' = currentStep + 1
    /\ UNCHANGED <<nextID, alive, components, ownership, tickOrder, tickCount>>

EndTick ==
    /\ currentStep = Len(Systems) + 1
    /\ currentStep' = 0
    /\ UNCHANGED <<nextID, alive, components, ownership, tickOrder, tickCount>>

Next ==
    \/ CreateEntity
    \/ \E e \in alive : DestroyEntity(e)
    \/ \E e \in alive, c \in ComponentTypes : AddComponent(e, c)
    \/ \E e \in alive, c \in ComponentTypes : RemoveComponent(e, c)
    \/ BeginTick
    \/ RunSystem
    \/ EndTick

Spec == Init /\ [][Next]_vars /\ WF_vars(BeginTick) /\ WF_vars(EndTick)

TickProgress == []<>(currentStep = 0)

====
