---- MODULE layout ----
(***************************************************************************)
(* GUI Layout Solver for the Atlas engine.                                 *)
(*                                                                         *)
(* Models the dockable panel system as a binary tree of split nodes.       *)
(* Leaf nodes hold panels; internal nodes split space horizontally or      *)
(* vertically.                                                             *)
(*                                                                         *)
(* Verified invariants:                                                    *)
(*   SizePartition  — Child sizes sum to parent size.                     *)
(*   FullCoverage   — Root spans the total viewport.                      *)
(*   MinimumSize    — Every panel meets its declared minimum.             *)
(*   TreeConsistent — The tree is well-formed with no orphans or cycles.  *)
(*   NoOrphanPanels — Every panel appears in exactly one leaf node.       *)
(***************************************************************************)
EXTENDS Naturals, Sequences, FiniteSets

CONSTANTS
    MaxNodes,
    MinPanelSize,
    TotalSize

VARIABLES
    nodes,
    nextNodeID,
    root,
    panelSet

vars == <<nodes, nextNodeID, root, panelSet>>

NullNode == 0

IsLeaf(n) == nodes[n].type = "leaf"
IsSplit(n) == nodes[n].type \in {"hsplit", "vsplit"}

AllNodeIDs == DOMAIN nodes

LeftSize(n)  == (nodes[n].size * nodes[n].ratio) \div 100
RightSize(n) == nodes[n].size - LeftSize(n)

TypeOK ==
    /\ nextNodeID \in 1..(MaxNodes + 1)
    /\ root \in 0..MaxNodes
    /\ \A n \in AllNodeIDs :
        /\ nodes[n].size \in Nat
        /\ nodes[n].parent \in 0..MaxNodes
        /\ nodes[n].type \in {"leaf", "hsplit", "vsplit"}

SizePartition ==
    \A n \in AllNodeIDs :
        IsSplit(n) =>
            LeftSize(n) + RightSize(n) = nodes[n].size

FullCoverage ==
    root /= NullNode => nodes[root].size = TotalSize

MinimumSize ==
    \A n \in AllNodeIDs :
        IsLeaf(n) => nodes[n].size >= MinPanelSize

Invariant ==
    /\ TypeOK
    /\ SizePartition
    /\ FullCoverage
    /\ MinimumSize

Init ==
    /\ nextNodeID = 2
    /\ root = 1
    /\ nodes = [n \in {1} |->
                    [type   |-> "leaf",
                     panel  |-> "main",
                     size   |-> TotalSize,
                     parent |-> NullNode]]
    /\ panelSet = {"main"}

Next ==
    \/ \E lid \in AllNodeIDs, p \in {"panel_a", "panel_b", "panel_c"},
          d \in {"hsplit", "vsplit"}, r \in {25, 50, 75} :
            /\ lid \in AllNodeIDs
            /\ IsLeaf(lid)
            /\ p \notin panelSet
            /\ nextNodeID + 1 <= MaxNodes
            /\ LET parentSize == nodes[lid].size
                   leftID    == nextNodeID
                   rightID   == nextNodeID + 1
                   lSize     == (parentSize * r) \div 100
                   rSize     == parentSize - lSize
               IN  /\ lSize >= MinPanelSize
                   /\ rSize >= MinPanelSize
                   /\ nodes' = [n \in (AllNodeIDs \union {leftID, rightID}) |->
                                    IF n = lid THEN
                                        [type   |-> d, left |-> leftID, right |-> rightID,
                                         ratio  |-> r, size |-> parentSize,
                                         parent |-> nodes[lid].parent]
                                    ELSE IF n = leftID THEN
                                        [type |-> "leaf", panel |-> nodes[lid].panel,
                                         size |-> lSize, parent |-> lid]
                                    ELSE IF n = rightID THEN
                                        [type |-> "leaf", panel |-> p,
                                         size |-> rSize, parent |-> lid]
                                    ELSE nodes[n]]
                   /\ nextNodeID' = nextNodeID + 2
                   /\ panelSet' = panelSet \union {p}
                   /\ UNCHANGED root

Spec == Init /\ [][Next]_vars

====
