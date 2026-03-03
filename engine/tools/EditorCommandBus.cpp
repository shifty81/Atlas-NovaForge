#include "EditorCommandBus.h"
#include "../core/Logger.h"

namespace atlas::tools {

void EditorCommandBus::PostCommand(std::unique_ptr<IEditorCommand> cmd) {
    if (cmd)
        m_pending.push_back(std::move(cmd));
}

void EditorCommandBus::ProcessCommands() {
    // Move pending commands into a local list so callers can safely post
    // new commands while we iterate.
    std::vector<std::unique_ptr<IEditorCommand>> batch;
    batch.swap(m_pending);

    for (auto& cmd : batch) {
        cmd->Execute();
        Logger::Info("EditorCommandBus: " + cmd->Description());

        if (m_listener)
            m_listener(*cmd, /*undone=*/false);

        // Posting a new command invalidates the redo history.
        m_redoStack.clear();
        m_undoStack.push_back(std::move(cmd));
    }
}

bool EditorCommandBus::UndoLast() {
    if (m_undoStack.empty()) return false;

    auto cmd = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    cmd->Undo();
    Logger::Info("EditorCommandBus: undo " + cmd->Description());

    if (m_listener)
        m_listener(*cmd, /*undone=*/true);

    m_redoStack.push_back(std::move(cmd));
    return true;
}

bool EditorCommandBus::RedoLast() {
    if (m_redoStack.empty()) return false;

    auto cmd = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    cmd->Execute();
    Logger::Info("EditorCommandBus: redo " + cmd->Description());

    if (m_listener)
        m_listener(*cmd, /*undone=*/false);

    m_undoStack.push_back(std::move(cmd));
    return true;
}

size_t EditorCommandBus::PendingCount() const {
    return m_pending.size();
}

size_t EditorCommandBus::UndoCount() const {
    return m_undoStack.size();
}

size_t EditorCommandBus::RedoCount() const {
    return m_redoStack.size();
}

void EditorCommandBus::Clear() {
    m_pending.clear();
    m_undoStack.clear();
    m_redoStack.clear();
}

void EditorCommandBus::SetListener(Listener listener) {
    m_listener = std::move(listener);
}

} // namespace atlas::tools
