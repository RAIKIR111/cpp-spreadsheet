#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <map>
#include <set>

class Sheet : public SheetInterface {
public:
    struct Hasher { 
        size_t operator()(const Position& position) const { 
            return std::hash<int>()(position.col) + std::hash<int>()(position.row) * 66; 
        } 
    };

public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    std::unordered_map<Position, CellInterface*, Hasher> data_;

    std::map<int, std::set<int>> row_to_cellindexes_;
};
