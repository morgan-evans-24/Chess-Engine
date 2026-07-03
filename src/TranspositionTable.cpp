//
// Created by Mercury on 03/07/2026.
//

#include "../include/TranspositionTable.h"


TranspositionTable::TranspositionTable(size_t sizeMB) {

    size_t sizeInBytes = sizeMB * 1024 * 1024;
    size_t rawSize = sizeInBytes / sizeof(TTEntry);

    tableSize = 1;
    while (tableSize < rawSize) {
        tableSize <<= 1;
    }

    table.resize(tableSize);
}

void TranspositionTable::store(uint64_t key, int depth, int score, Move bestMove, Flag flag) {
    auto entry = TTEntry(key, depth, score, bestMove, flag, true);
    int index = key & (tableSize - 1);
    table[index] = entry;

}

TranspositionTable::TTEntry TranspositionTable::search(uint64_t key) const {
    int index = key & (tableSize - 1);
    const TTEntry& entry = table[index];

    if (entry.valid && entry.key == key) {
        return entry;
    }

    return {};
}
