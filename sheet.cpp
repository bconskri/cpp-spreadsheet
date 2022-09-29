#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Wrong cell coordinates");
    }
    //
    auto s = Size{pos.row + 1, pos.col + 1};
    if (cells_on_sheet_.size() < (size_t) s.rows) {
        //добавим строки до нужной позиции
        cells_on_sheet_.resize(s.rows);
    }
    if (cells_on_sheet_[pos.row].size() < (size_t) s.cols) {
        //добавим колонки до нужной позиции в нужной строке
        cells_on_sheet_[pos.row].resize(s.cols);
    }
    if (size_cached_.rows < s.rows) {
        //нужно увеличить обьем печатной области
        size_cached_.rows = s.rows;
    }
    if (size_cached_.cols < s.cols) {
        //нужно увеличить обьем печатной области
        size_cached_.cols = s.cols;
    }
    auto &cell = cells_on_sheet_[pos.row][pos.col];
    if (!cell) {
        cell = CreateCell(*this);
    }
    cell->Set(text);
}

const CellInterface *Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Wrong cell coordinates");
    }
    if ((size_t) pos.row < cells_on_sheet_.size() && (size_t) pos.col < cells_on_sheet_[pos.row].size()) {
        return cells_on_sheet_[pos.row][pos.col].get();
    }
    return nullptr;
}

CellInterface *Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Wrong cell coordinates");
    }
    if ((size_t) pos.row < cells_on_sheet_.size() && (size_t) pos.col < cells_on_sheet_[pos.row].size()) {
        return cells_on_sheet_[pos.row][pos.col].get();
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Wrong cell coordinates");
    }
    if ((size_t) pos.row < cells_on_sheet_.size() && (size_t) pos.col < cells_on_sheet_[pos.row].size()) { //проверим наличие ячейки
        cells_on_sheet_[pos.row][pos.col].release();
        //удалим все пустые ячейки в строке от текущей очищенной влево
        size_t i = pos.col;
        while (cells_on_sheet_[pos.row].size() > 0 && !cells_on_sheet_[pos.row][i]) {
            cells_on_sheet_[pos.row].pop_back();
            --i;
        }
        //обновим размер печатной области
        //удалим снизу все пустые строки
        i = cells_on_sheet_.size() > 0 ? cells_on_sheet_.size() - 1 : 0;
        while (cells_on_sheet_.size() > 0 && cells_on_sheet_[i].size() == 0) {
            cells_on_sheet_.pop_back();
            --i;
        }
        size_cached_.rows = cells_on_sheet_.size();
        //найдем макс длины столбец
        size_t max = 0;
        for (size_t i = 0; i < cells_on_sheet_.size(); ++i) {
            if (cells_on_sheet_[i].size() > max) {
                max = cells_on_sheet_[i].size();
            }
        }
        //по нему ограничим область
        size_cached_.cols = max;
    }
}

Size Sheet::GetPrintableSize() const {
    return size_cached_;
}

void Sheet::PrintValues(std::ostream &output) const {
    for (const auto &r : cells_on_sheet_) {
        bool first = true;
        for (const auto &c : r) {
            if (!first) {
                output << '\t';
            }
            first = false;
            if (c) {
                output << c->GetValue();
            }
        }
        //напечатаем отступов до размера печатной области
        for (size_t i = 1; i < (size_cached_.cols - r.size()); ++i) {
            output << '\t';
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream &output) const {
    for (const auto &r : cells_on_sheet_) {
        bool first = true;
        for (const auto &c : r) {
            if (!first) {
                output << '\t';
            }
            first = false;
            if (c) {
                output << c->GetText();
            }
        }
        //напечатаем отступов до размера печатной области
        for (size_t i = 1; i < (size_cached_.cols - r.size()); ++i) {
            output << '\t';
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}