#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit([&](const auto& x) { output << x; }, value);
    return output;
}

class Cell::Impl {
public:
    Impl() = default;

    virtual ~Impl() = default;

    virtual Value GetValue() const = 0;

    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const {
        return {};
    }
};

class Cell::EmptyImpl : public Impl {
public:
    Value GetValue() const override {
        return ""s;
    }

    std::string GetText() const override {
        return ""s;
    }
};

class Cell::TextImpl : public Impl {
public:
    explicit TextImpl(std::string str) : text_(std::move(str)) {
        if (text_.empty()) {
            throw std::invalid_argument("Empty string for TextCell");
        }
    }

    Value GetValue() const override {
        if (text_[0] == ESCAPE_SIGN) {
            return text_.substr(1);
        }
        return text_;
    }

    std::string GetText() const override {
        return text_;
    }

private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
    explicit FormulaImpl(std::string str, const SheetInterface &sheet)
            : formula_(ParseFormula(std::move(str)))
            , sheet_(sheet) {}

    Value GetValue() const override {
        auto cell_interface_value = formula_->Evaluate(sheet_);
        return std::visit(
                [](const auto &formula_interface_value) {return Value(formula_interface_value);}
                , cell_interface_value);
    }

    std::string GetText() const override {
        return FORMULA_SIGN + formula_->GetExpression();
    }

    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
    const SheetInterface &sheet_;
    std::optional<FormulaInterface::Value> cache_;
};

Cell::Cell(SheetInterface& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(const std::string& text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
        return;
    }
    //Текст, который содержит только знак "=", формулой не считается, а считается текстом
    if (text[0] == FORMULA_SIGN && text.size() > 1) {
        auto impl = std::make_unique<FormulaImpl>(text.substr(1), sheet_);
        //check circular dependency
        if (CircularDependency(*impl)) {
            throw CircularDependencyException("Circular dependency");
        }
        impl_ = std::move(impl);
        //
        auto referenced_cells = impl_->GetReferencedCells();
        //create outref cells  if not exist
        for (auto& cell : referenced_cells) {
            if (!sheet_.GetCell(cell)) {
                sheet_.SetCell(cell, {});
            }
        }
        //
        FillCellsRefs();
        //
        return;
    }
    impl_ = std::make_unique<TextImpl>(text);
}

void Cell::Clear() {
    Set(""); //impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !inRefCells_.empty();
}

bool Cell::CircularDependency(const Cell::Impl &impl_) const {
    auto get_referenced_cells = impl_.GetReferencedCells();
    if (get_referenced_cells.empty()) {
        //no dependencies
        return false;
    }
    //make set of all cells in formula
    std::unordered_set<const CellInterface*> refs;
    for (const auto &pos : get_referenced_cells) {
        refs.insert(sheet_.GetCell(pos));
    }
    //visit all cells and check circular references
    std::unordered_set<const CellInterface*> visited;
    std::stack<const CellInterface*> to_visit;
    to_visit.push(this);
    while (!to_visit.empty()) {
        const CellInterface *current = to_visit.top();
        to_visit.pop();
        visited.insert(current);
        if (refs.find(current) != refs.end()) {
            return true;
        }
        auto& ref_cells = static_cast<const Cell*>(current)->inRefCells_;
        for (auto& incoming : ref_cells) {
            if (visited.find(incoming) == visited.end()) {
                to_visit.push(incoming);
            }
        }
    }
    return false;
}

void Cell::FillCellsRefs() {
    //first clear inref in all outgoing ref cells
    for (auto& outgoing : outRefCells_) {
        outgoing->inRefCells_.erase(this);
    }
    outRefCells_.clear();
    for (const auto &pos : impl_->GetReferencedCells()) {
        auto outgoing = reinterpret_cast<Cell*>(sheet_.GetCell(pos));
        //update cross links
        outRefCells_.insert(outgoing);
        outgoing->inRefCells_.insert(this);
    }
}

void Cell::InvalidateCache() {

}

std::unique_ptr<Cell> CreateCell(SheetInterface &sheet) {
    return std::make_unique<Cell>(sheet);
}