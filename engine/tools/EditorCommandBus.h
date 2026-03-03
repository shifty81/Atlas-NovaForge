#pragma once
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <vector>

namespace atlas::tools {

/// Abstract base for editor commands.
///
/// Every mutation that the ToolingLayer performs on the simulation must be
/// wrapped in an IEditorCommand and posted to the EditorCommandBus.  This
/// guarantees that:
///   1. The editor never mutates simulation data directly.
///   2. Every action is undoable / redoable.
///   3. Commands are deterministic and replayable.
struct IEditorCommand {
    virtual ~IEditorCommand() = default;

    /// Human-readable description (e.g. "Move Entity 'ship_01' to (100, 200)").
    virtual std::string Description() const = 0;

    /// Apply the command to the simulation.
    virtual void Execute() = 0;

    /// Reverse the effect of Execute().
    virtual void Undo() = 0;
};

/// Queues, executes, and manages undo/redo for editor commands.
///
/// Usage:
///   1. Tool posts a command via PostCommand().
///   2. Game loop calls ProcessCommands() once per frame.
///   3. User presses Ctrl+Z → UndoLast().
///   4. User presses Ctrl+Y → RedoLast().
///
/// Thread safety: PostCommand() is safe to call from any thread (it only
/// appends to the pending queue under a lock).  All other methods must be
/// called from the main/game thread.
class EditorCommandBus {
public:
    /// Queue a command for execution on the next ProcessCommands() call.
    void PostCommand(std::unique_ptr<IEditorCommand> cmd);

    /// Execute all pending commands.  Each executed command is pushed onto
    /// the undo stack.  Posting a new command clears the redo stack.
    void ProcessCommands();

    /// Undo the most recently executed command.
    /// Returns false if the undo stack is empty.
    bool UndoLast();

    /// Redo the most recently undone command.
    /// Returns false if the redo stack is empty.
    bool RedoLast();

    /// Number of commands waiting to be executed.
    size_t PendingCount() const;

    /// Number of commands on the undo stack.
    size_t UndoCount() const;

    /// Number of commands on the redo stack.
    size_t RedoCount() const;

    /// Remove all pending, undo, and redo commands.
    void Clear();

    /// Optional callback invoked after each command is executed, undone, or
    /// redone.  Useful for logging or UI refresh.
    using Listener = std::function<void(const IEditorCommand& cmd, bool undone)>;
    void SetListener(Listener listener);

private:
    std::vector<std::unique_ptr<IEditorCommand>> m_pending;
    std::vector<std::unique_ptr<IEditorCommand>> m_undoStack;
    std::vector<std::unique_ptr<IEditorCommand>> m_redoStack;
    Listener m_listener;
};

} // namespace atlas::tools
