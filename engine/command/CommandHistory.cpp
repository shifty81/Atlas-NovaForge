#include "CommandHistory.h"
#include "../core/Logger.h"

namespace atlas::command {

void CommandHistory::Execute(std::unique_ptr<Command> cmd) {
    cmd->Execute();
    Logger::Info("Command executed: " + cmd->Name());
    m_undoStack.push_back(std::move(cmd));
    m_redoStack.clear();
}

bool CommandHistory::CanUndo() const {
    return !m_undoStack.empty();
}

bool CommandHistory::CanRedo() const {
    return !m_redoStack.empty();
}

void CommandHistory::Undo() {
    if (m_undoStack.empty()) return;

    auto cmd = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    cmd->Undo();
    Logger::Info("Command undone: " + cmd->Name());
    m_redoStack.push_back(std::move(cmd));
}

void CommandHistory::Redo() {
    if (m_redoStack.empty()) return;

    auto cmd = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    cmd->Execute();
    Logger::Info("Command redone: " + cmd->Name());
    m_undoStack.push_back(std::move(cmd));
}

void CommandHistory::Clear() {
    m_undoStack.clear();
    m_redoStack.clear();
}

size_t CommandHistory::UndoCount() const {
    return m_undoStack.size();
}

size_t CommandHistory::RedoCount() const {
    return m_redoStack.size();
}

const Command* CommandHistory::LastExecuted() const {
    if (m_undoStack.empty()) return nullptr;
    return m_undoStack.back().get();
}

}
