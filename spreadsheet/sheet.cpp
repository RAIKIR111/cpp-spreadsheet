#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {
    for (auto& item : data_) {
        delete item.second;
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Position is invalid!");
    }
    CellInterface*& temp_cell = data_[pos];
    if (temp_cell == nullptr) {
        temp_cell = new Cell(*this);
    }
    Cell* cell = dynamic_cast<Cell*>(temp_cell);
    cell->Set(text, pos);
    row_to_cellindexes_[pos.row].insert(pos.col);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Position is invalid!");
    }
    if (data_.count(pos) == 0) {
        return nullptr;
    }

    return data_.at(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Position is invalid!");
    }
    if (data_.count(pos) == 0) {
        return nullptr;
    }

    return data_.at(pos);
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Position is invalid!");
    }
    if (data_.count(pos) == 0) {
        return;
    }
    CellInterface*& temp_cell = data_.at(pos);
    delete temp_cell;
    data_.erase(pos);
    row_to_cellindexes_[pos.row].erase(pos.col);
}

Size Sheet::GetPrintableSize() const {
    if (data_.size() == 0) {
        return Size{};
    }

    int max_col = 0;
    int max_row = 0;
    bool max_row_search_flag = true;
    for (auto it = row_to_cellindexes_.rbegin(); it != row_to_cellindexes_.rend(); ++it) {
        if (it->second.size() != 0 && (max_col < *prev(it->second.end()) || max_col == 0)) {
            max_col = *prev(it->second.end()) + 1;
        }
        
        if (max_row_search_flag == true && it->second.size() != 0) {
            max_row = it->first + 1;
            max_row_search_flag = false;
        }
    }
    return {max_row, max_col};
}

void Sheet::PrintValues(std::ostream& output) const {
    const Size data_size = GetPrintableSize();
    for (auto counter = 0; counter < data_size.rows; ++counter) {
        bool flag = false;
        for (auto in_counter = 0; in_counter < data_size.cols; ++in_counter) {
            const CellInterface* temp_cell = GetCell({counter, in_counter});
            if (flag == false) {
                flag = true;
            }
            else {
                output << '\t';
            }
            if (temp_cell != nullptr) {
                const auto temp_val = temp_cell->GetValue();
                if (std::holds_alternative<std::string>(temp_val)) {
                    output << std::get<std::string>(data_.at({counter, in_counter})->GetValue());
                }
                else if (std::holds_alternative<double>(temp_val)) {
                    output << std::get<double>(data_.at({counter, in_counter})->GetValue());
                }
                else {
                    output << std::get<FormulaError>(data_.at({counter, in_counter})->GetValue()).ToString();
                }
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    const Size data_size = GetPrintableSize();
    for (auto counter = 0; counter < data_size.rows; ++counter) {
        bool flag = false;
        for (auto in_counter = 0; in_counter < data_size.cols; ++in_counter) {
            const CellInterface* temp_cell = GetCell({counter, in_counter});
            if (flag == false) {
                flag = true;
            }
            else {
                output << '\t';
            }
            if (temp_cell != nullptr) {
                output << temp_cell->GetText();
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}