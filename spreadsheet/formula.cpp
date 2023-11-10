#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>

using namespace std::literals;

// ------------ FormulaError ----------------------------
FormulaError::FormulaError(Category category)
    : category_(category) {
}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!"s;
        case Category::Value:
            return "#VALUE!"s;
        case Category::Div0:
            return "#ARITHM!"s;
        default:
            return {};
    }
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(expression)) {
    }

    Value Evaluate(const SheetInterface& sheet_interface) const override {
        try {
            return ast_.Execute([&sheet_interface](Position pos) {
                const CellInterface* cell_ptr = nullptr;
                try {
                    cell_ptr = sheet_interface.GetCell(pos);
                } catch (const InvalidPositionException&) {
                    throw FormulaError(FormulaError::Category::Ref);
                }
                if (!cell_ptr) {
                    return 0.0;
                }
                const auto& temp_value = cell_ptr->GetValue();
                if (std::holds_alternative<double>(temp_value)) {
                    return std::get<double>(temp_value);
                }
                else if (std::holds_alternative<std::string>(temp_value)) {
                    const auto& str = cell_ptr->GetText();
                    if (str.empty()) {
                        return 0.0;
                    }
                    if (str[0] == ESCAPE_SIGN) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                    std::regex double_pattern("^(-?)(0|([1-9][0-9]*))(\\.[0-9]+)?$");
                    if (!std::regex_match(str, double_pattern)) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                    return std::stod(str);
                }
                throw std::get<FormulaError>(temp_value);
            });
        } catch (const FormulaError& formula_error) {
            return formula_error;
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

    std::vector<Position> GetReferencedCells() const {
        const auto& temp_cells = ast_.GetCells();
        std::vector<Position> cells = {temp_cells.begin(), temp_cells.end()};
        auto last = std::unique(cells.begin(), cells.end());
        cells.erase(last, cells.end());
        return cells;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}