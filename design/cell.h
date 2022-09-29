#pragma once

#include "common.h"
#include "formula.h"

std::ostream &operator<<(std::ostream &output, const CellInterface::Value &value);

class Cell : public CellInterface {
public:
    Cell();

    ~Cell();

    void Set(const std::string& text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

private:
    class Impl;

    class EmptyImpl;

    class TextImpl;

    class FormulaImpl;

    std::unique_ptr<Impl> impl_;

    std::unordered_set<std::unique_ptr<Cell>> outRefCells_;
    std::unordered_set<std::unique_ptr<Cell>> inRefCells_;
    Sheet &sheet_;

    //поиск циклической зависимости
    //обход графа по зависимым ячейкам начиная с impl
    bool CheckCircularDependency(const Impl &impl_) const;

    //обновление зависимосте по ячейке
    void UpdateCellsRefs();

    //инвалидировать кэш рекурсивно по зависимым
    void InvalidateCache();
};