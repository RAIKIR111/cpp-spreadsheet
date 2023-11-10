#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>
#include <stack>

class Sheet;

class Cell : public CellInterface {
public:
    struct Hasher { 
        size_t operator()(const Position& position) const { 
            return std::hash<int>()(position.col) + std::hash<int>()(position.row) * 66; 
        } 
    };

private:
    class Impl {
    public:
        virtual ~Impl() = default;

        virtual Value GetValue(const SheetInterface&) const = 0;

        virtual std::string GetText() const = 0;

        virtual bool IsReferenced() const = 0;
    };

    class TextImpl : public Impl {
    public:
        TextImpl(std::string expr) {
            text_ = expr;
        }

        Value GetValue(const SheetInterface&) const override {
            if (text_.at(0) == ESCAPE_SIGN) {
                return std::string{next(text_.begin()), text_.end()};
            }
            return text_;
        }

        std::string GetText() const override {
            return text_;
        }

        bool IsReferenced() const override {
            return false;
        }

    private:
        std::string text_;
    };

    class FormulaImpl : public Impl {
    public:
        FormulaImpl(std::string expr, const SheetInterface& sheet_interface)
            : formula_interface_(ParseFormula(expr))
            , sheet_interface_(sheet_interface) {
        }

        CellInterface::Value GetValue(const SheetInterface&) const override {
            FormulaInterface::Value temp_value = formula_interface_->Evaluate(sheet_interface_);
            if (std::holds_alternative<FormulaError>(temp_value)) {
                return std::get<FormulaError>(temp_value);
            }

            return std::get<double>(temp_value);
        }

        std::string GetText() const override {
            return '=' + formula_interface_->GetExpression();
        }

        bool IsReferenced() const override {
            return !formula_interface_->GetReferencedCells().empty();
        }

        std::vector<Position> GetReferencedCells() const {
            return formula_interface_->GetReferencedCells();
        }

    private:
        std::unique_ptr<FormulaInterface> formula_interface_;
        const SheetInterface& sheet_interface_;
    };

    class BlankImpl : public Impl {
    public:
        Value GetValue(const SheetInterface&) const override {
            return Value{};
        }

        std::string GetText() const override {
            return std::string{};
        }

        bool IsReferenced() const override {
            return false;
        }
    };

public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text, Position pos);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    std::unique_ptr<Impl> impl_;

    SheetInterface& sheet_;
    mutable std::unique_ptr<Value> cache_;
    std::unordered_set<Cell*> influences_;
    Position pos_{-1, -1};

    void CheckIfCyclicDependency(const std::vector<Position>& deps) const;
    void RecursiveCheck(const std::vector<Position>& deps, std::unordered_set<Position, Hasher>& visited) const;

    void UpdateGraph(std::unique_ptr<Impl> new_impl);

    void InvalidateCache() const;

    void InvalidateReferencedCells();
    void UpdateReferencedCells(std::unique_ptr<Impl> new_impl);
};