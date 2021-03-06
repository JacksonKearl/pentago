// Board definitions and utility functions

#include <pentago/base/board.h>
#include <pentago/utility/debug.h>
#include <geode/array/Array2d.h>
#include <geode/array/NdArray.h>
#include <geode/math/popcount.h>
#include <geode/python/wrap.h>
#include <geode/random/Random.h>
#include <geode/utility/format.h>
#include <geode/utility/interrupts.h>
namespace pentago {

using namespace geode;
using std::cout;
using std::endl;

bool black_to_move(board_t board) {
  check_board(board);
  const side_t side0 = unpack(board,0),
               side1 = unpack(board,1);
  const int count0 = popcount(side0),
            count1 = popcount(side1);
  GEODE_ASSERT(count0==count1 || count1==count0+1);
  return count0==count1;
}

void check_board(board_t board) {
  #define CHECK(q) \
    if (!(quadrant(board,q)<(int)pow(3.,9.))) \
      THROW(ValueError,"quadrant %d has invalid value %d",q,quadrant(board,q));
  CHECK(0) CHECK(1) CHECK(2) CHECK(3)
}

static NdArray<board_t> pack_py(NdArray<const side_t> sides) {
  GEODE_ASSERT(sides.rank()>=1 && sides.shape.back()==2);
  NdArray<board_t> boards(sides.shape.slice(0,sides.rank()-1).copy(),uninit);
  for (int b=0;b<boards.flat.size();b++) {
    const side_t side0 = sides.flat[2*b],
                 side1 = sides.flat[2*b+1];
    GEODE_ASSERT(!(side0&side1) && !((side0|side1)&~side_mask));
    boards.flat[b] = pack(side0,side1);
  }
  return boards;
}

static NdArray<side_t> unpack_py(NdArray<const board_t> boards) {
  for (int b=0;b<boards.flat.size();b++)
    check_board(boards.flat[b]);
  Array<int> shape = boards.shape.copy(); 
  shape.append(2);
  NdArray<side_t> sides(shape,uninit);
  for (int b=0;b<boards.flat.size();b++)
    for (int i=0;i<2;i++)
      sides.flat[2*b+i] = unpack(boards.flat[b],i);
  return sides;
}

Array<int,2> to_table(const board_t board) {
  check_board(board);
  Array<int,2> table(6,6,uninit);
  for (int qx=0;qx<2;qx++) for (int qy=0;qy<2;qy++) {
    const quadrant_t q = quadrant(board,2*qx+qy);
    const side_t s0 = unpack(q,0),
                 s1 = unpack(q,1);
    for (int x=0;x<3;x++) for (int y=0;y<3;y++)
      table(3*qx+x,3*qy+y) = ((s0>>(3*x+y))&1)+2*((s1>>(3*x+y))&1);
  }
  return table;
}

NdArray<int> to_tables(NdArray<const board_t> boards) {
  for (int b=0;b<boards.flat.size();b++)
    check_board(boards.flat[b]);
  Array<int> shape = boards.shape.copy();
  shape.append(6);
  shape.append(6);
  NdArray<int> tables(shape,uninit);
  for (int b=0;b<boards.flat.size();b++) {
    for (int qx=0;qx<2;qx++) for (int qy=0;qy<2;qy++) {
      const quadrant_t q = quadrant(boards.flat[b],2*qx+qy);
      const side_t s0 = unpack(q,0),
                   s1 = unpack(q,1);
      for (int x=0;x<3;x++) for (int y=0;y<3;y++)
        tables.flat[36*b+6*(3*qx+x)+3*qy+y] = ((s0>>(3*x+y))&1)+2*((s1>>(3*x+y))&1);
    }
  }
  return tables;
}

static NdArray<board_t> from_table(NdArray<const int> tables) {
  GEODE_ASSERT(tables.rank()>=2);
  int r = tables.rank();
  GEODE_ASSERT(tables.shape[r-2]==6 && tables.shape[r-1]==6);
  NdArray<board_t> boards(tables.shape.slice(0,r-2).copy(),uninit);
  for (int b=0;b<boards.flat.size();b++) {
    quadrant_t q[4];
    for (int qx=0;qx<2;qx++) for (int qy=0;qy<2;qy++) {
      quadrant_t s0=0,s1=0;
      for (int x=0;x<3;x++) for (int y=0;y<3;y++) {
        quadrant_t bit = 1<<(3*x+y);
        switch (tables.flat[36*b+6*(3*qx+x)+3*qy+y]) {
          case 1: s0 |= bit; break;
          case 2: s1 |= bit; break;
        }
      }
      q[2*qx+qy] = pack(s0,s1);
    }
    boards.flat[b] = quadrants(q[0],q[1],q[2],q[3]);
  }
  return boards;
}

static inline board_t pack(const Vector<Vector<quadrant_t,2>,4>& sides) {
    return quadrants(pack(sides[0][0],sides[0][1]),
                     pack(sides[1][0],sides[1][1]),
                     pack(sides[2][0],sides[2][1]),
                     pack(sides[3][0],sides[3][1]));
}

// Rotate and reflect a board to minimize its value
board_t standardize(board_t board) {
  Vector<Vector<quadrant_t,2>,4> sides;
  for (int q=0;q<4;q++) for (int s=0;s<2;s++)
    sides[q][s] = unpack(quadrant(board,q),s);
  board_t transformed[8];
  for (int rotation=0;rotation<4;rotation++) {
    for (int reflection=0;reflection<2;reflection++) {
      transformed[2*rotation+reflection] = pack(sides);
      // Reflect about x = y line
      for (int q=0;q<4;q++)
        for (int s=0;s<2;s++)
          sides[q][s] = reflections[sides[q][s]];
      swap(sides[0],sides[3]);
    }
    // Rotate left
    for (int q=0;q<4;q++)
      for (int s=0;s<2;s++)
        sides[q][s] = rotations[sides[q][s]][0];
    Vector<Vector<quadrant_t,2>,4> prev = sides;
    sides[0] = prev[1];
    sides[1] = prev[3];
    sides[2] = prev[0];
    sides[3] = prev[2];
  }
  return RawArray<board_t>(8,transformed).min();
}

static NdArray<board_t> standardize_py(NdArray<const board_t> boards) {
  NdArray<board_t> transformed(boards.shape,uninit);
  for (int b=0;b<boards.flat.size();b++)
    transformed.flat[b] = standardize(boards.flat[b]);
  return transformed;
}

side_t random_side(Random& random) {
  return random.bits<uint64_t>()&side_mask;
}

board_t random_board(Random& random) {
  side_t filled = random_side(random);
  side_t black = random.bits<uint64_t>()&filled;
  return pack(black,filled^black);
}

static side_t add_random_stone(Random& random, side_t filled) {
  for (;;) {
    int j = random.uniform<int>(0,36);
    side_t stone = (side_t)1<<(16*(j&3)+j/4);
    if (!(stone&filled)) {
      GEODE_ASSERT(popcount(filled)+1==popcount(stone|filled));
      return stone|filled;
    }
  }
}

// Generate a random board with n stones
board_t random_board(Random& random, int n) {
  const int nw = n/2, nb = n-nw, ne = 36-n;
  // Randomly place white stones
  side_t white = 0;
  for (int i=0;i<nw;i++)
    white = add_random_stone(random,white);
  // Random place either black stones or empty
  const int no = min(nb,ne);
  side_t other = white;
  for (int i=0;i<no;i++)
    other = add_random_stone(random,other);
  // Construct board
  const side_t black = no==nb?other&~white:side_mask&~other;
  GEODE_ASSERT(popcount(white)==nw);
  GEODE_ASSERT(popcount(black)==nb);
  GEODE_ASSERT(popcount(white|black)==n);
  return pack(black,white);
}

string str_board(board_t board) {
  string s;
  s += format("counts: 0s = %d, 1s = %d\n\n",popcount(unpack(board,0)),popcount(unpack(board,1)));
  const Array<const int,2> table = to_table(board);
  for (int i=0;i<6;i++) {
    int y = 5-i;
    s += "abcdef"[i];
    s += "  ";
    for (int x=0;x<6;x++)
      s += "_01"[table(x,y)];
    s += '\n';
  }
  return s+"\n   123456";
}

}
using namespace pentago;
using namespace geode::python;

void wrap_board() {
  function("unpack",unpack_py);
  function("pack",pack_py);
  function("standardize",standardize_py);
  GEODE_FUNCTION_2(to_table,to_tables)
  GEODE_FUNCTION(from_table)
  GEODE_FUNCTION(check_board)
  GEODE_FUNCTION(black_to_move)
  GEODE_FUNCTION_2(flip_board_py,flip_board)
}
