#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

using namespace std::literals;

Cell::Cell(SheetInterface& sheet)
    : impl_(new BlankImpl())
    , sheet_(sheet) {
}

Cell::~Cell() = default;

void Cell::Set(std::string text, Position pos) {
    pos_ = pos;
    std::unique_ptr<Impl> temp_impl;
    if (!text.empty()) {
        if (text.at(0) == FORMULA_SIGN && text.size() > 1) {
            try {
                temp_impl = std::make_unique<FormulaImpl>(std::string{next(text.begin()), text.end()}, sheet_);
            }
            catch (const FormulaException& formula_execption) {
                throw formula_execption;
            }
            CheckIfCyclicDependency(dynamic_cast<FormulaImpl*>(temp_impl.get())->GetReferencedCells());
        } else {
            temp_impl = std::make_unique<TextImpl>(text);
        }
    } else {
        temp_impl = std::make_unique<BlankImpl>();
    }
    UpdateGraph(std::move(temp_impl));
}

void Cell::Clear() {
    impl_.reset();
    cache_.reset();
}

Cell::Value Cell::GetValue() const {
    if (!cache_) {
        const auto& temp_value = impl_->GetValue(sheet_);
        if (std::holds_alternative<std::string>(temp_value)) {
            return temp_value;
        }
        cache_ = std::make_unique<Value>(temp_value);
    }
    return *cache_;
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

bool Cell::IsReferenced() const {
    return impl_->IsReferenced();
}

std::vector<Position> Cell::GetReferencedCells() const {
    if (!IsReferenced()) {
        return {};
    }
    return dynamic_cast<FormulaImpl*>(impl_.get())->GetReferencedCells();
}

void Cell::CheckIfCyclicDependency(const std::vector<Position>& new_dependences) const {
    std::unordered_set<Position, Hasher> visited = {pos_};
    RecursiveCheck(new_dependences, visited);
}

void Cell::RecursiveCheck(const std::vector<Position>& deps, std::unordered_set<Position, Hasher>& visited) const {
    for (const Position& temp_pos : deps) {
        if (visited.count(temp_pos) == 0) {
            visited.insert(temp_pos);
            auto temp_cell = sheet_.GetCell(temp_pos);
            if (temp_cell) {
                dynamic_cast<Cell*>(temp_cell)->RecursiveCheck(temp_cell->GetReferencedCells(), visited);
            }
        }
        else {
            throw CircularDependencyException("");
        }
    }
}

void Cell::UpdateGraph(std::unique_ptr<Impl> new_impl) {
    InvalidateCache();
    InvalidateReferencedCells();
    UpdateReferencedCells(std::move(new_impl));
}

void Cell::InvalidateCache() const {
    cache_.reset();
    std::unordered_set<Cell*> visited;
    for (const auto& item : influences_) {
        if (visited.count(item) == 0) {
            item->InvalidateCache();
            visited.insert(item);
        }
    }
}

void Cell::InvalidateReferencedCells() {
    Cell* cell = nullptr;
    for (const Position& pos : GetReferencedCells()) {
        cell = dynamic_cast<Cell*>(sheet_.GetCell(pos));
        cell->influences_.erase(this);
    }
}

void Cell::UpdateReferencedCells(std::unique_ptr<Impl> new_impl) {
    Cell* cell = nullptr;
    impl_ = std::move(new_impl);
    for (const Position& pos : GetReferencedCells()) {
        if (!sheet_.GetCell(pos)) {
            sheet_.SetCell(pos, "");
        }
        cell = dynamic_cast<Cell*>(sheet_.GetCell(pos));
        cell->influences_.insert(this);
    }
}