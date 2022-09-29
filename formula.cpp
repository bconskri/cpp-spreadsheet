#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream &operator<<(std::ostream &output, FormulaError fe) {
    return output << fe.ToString();
}

FormulaError::FormulaError(Category category) : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!"sv;
        case Category::Value:
            return "#VALUE!"sv;
        case Category::Div0:
            return "#DIV/0!"sv;
    }
    return ""sv;
}

namespace {
    double NumericOrErrorFromValue(const std::string &str) {
        double value = 0;
        if (!str.empty()) {
            std::istringstream input(str);
            if (!(input >> value) || !input.eof()) {
                throw FormulaError(FormulaError::Category::Value);
            }
        }
        return value;
    }

    double NumericOrErrorFromValue(FormulaError error) {
        throw FormulaError(error);
    }

    double NumericOrErrorFromValue(double value) {
        return value;
    }

    double GetValueFromVisit(const CellInterface *cell) {
        if (cell) {
            return std::visit([](const auto &value) { return NumericOrErrorFromValue(value); },
                              cell->GetValue());
        }
        return 0;
    }

    class Formula : public FormulaInterface {
    public:
        explicit Formula(std::string expression) : ast_(ParseFormulaAST(expression)) {}

        Value Evaluate(const SheetInterface &sheet) const override { //Value = std::variant<double, FormulaError>;
            try {
                auto cell_by_position = [&sheet](const Position position) -> double {
                    if (!position.IsValid()) {
                        throw FormulaError(FormulaError::Category::Ref);
                    }
                    const auto *cell = sheet.GetCell(position);
                    return GetValueFromVisit(cell);
                };

                return ast_.Execute(cell_by_position);
            } catch (FormulaError &error) {
                return error;
            }
        }

        std::string GetExpression() const override {
            std::ostringstream out;
            ast_.PrintFormula(out);
            return out.str();
        }

        std::vector<Position> GetReferencedCells() const override {
            std::set<Position> ref_cells;
            for (const auto& cell : ast_.GetCells()) {
                if (cell.IsValid()) {
                    ref_cells.emplace(cell);
                }
            }
            return {ref_cells.begin(), ref_cells.end()};
        }

    private:
        FormulaAST ast_;
    };
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}