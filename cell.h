#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

std::ostream &operator<<(std::ostream &output, const CellInterface::Value &value);

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(const std::string& text);

    void Clear();

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl;

    class EmptyImpl;

    class TextImpl;

    class FormulaImpl;

    std::unique_ptr<Impl> impl_;

    std::unordered_set<Cell*> outRefCells_;
    std::unordered_set<Cell*> inRefCells_;
    SheetInterface &sheet_;

    //поиск циклической зависимости
    //обход графа по зависимым ячейкам начиная с impl
    bool CircularDependency(const Impl &impl_) const;

    //обновление зависимосте по ячейке
    void FillCellsRefs();

    //инвалидировать кэш по зависимым
    void InvalidateCache();
};

//Создает пустую ячейку
std::unique_ptr<Cell> CreateCell(SheetInterface &sheet);