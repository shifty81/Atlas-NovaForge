#pragma once
#include <string>
#include <vector>
#include <memory>

namespace atlas::command {

class Command {
public:
    virtual ~Command() = default;

    virtual void Execute() = 0;
    virtual void Undo() = 0;

    virtual std::string Name() const = 0;
};

class CommandHistory {
public:
    void Execute(std::unique_ptr<Command> cmd);

    bool CanUndo() const;
    bool CanRedo() const;

    void Undo();
    void Redo();

    void Clear();

    size_t UndoCount() const;
    size_t RedoCount() const;

    const Command* LastExecuted() const;

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
};

}
