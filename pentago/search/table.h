// Hashed transposition table
//
// A naive hashed transposition table, with no symmetry awareness or rotation abstract.
// See supertable.h for a more useful version.
#pragma once

// Notes:
// 1. Collisions are resolved simply: the entry with greater depth wins.
// 2. Since our hash is bijective, we store only the high order bits of the hash to detect collisions.
//    This is compatible with zero initialization since hash_board(board)!=0 for all valid boards.

#include <pentago/base/board.h>
#include <pentago/base/score.h>
namespace pentago {

// Initialize a empty table with size 1<<bits entries
extern void init_table(int bits);

// Different kinds of tables
enum table_type_t {blank_table,normal_table,simple_table};

// Freeze the table into the given type
extern void set_table_type(table_type_t type);

// Lookup an entry in the table, returning a depth 0 tie (score(0,1)) if nothing is found
extern score_t lookup(board_t board);

// Store a new entry
extern void store(board_t board, score_t score);

}
