// Helper functions for testing purposes

#include <pentago/mpi/block_store.h>
#include <pentago/mpi/utility.h>
#include <pentago/convert.h>
#include <pentago/count.h>
#include <pentago/supertensor.h>
#include <pentago/symmetry.h>
#include <pentago/utility/char_view.h>
#include <pentago/utility/index.h>
#include <pentago/utility/memory.h>
#include <other/core/python/module.h>
#include <other/core/python/stl.h>
#include <other/core/utility/curry.h>
#include <other/core/utility/ProgressIndicator.h>
#include <other/core/utility/Hasher.h>
#include <other/core/utility/Log.h>
#include <tr1/unordered_map>
namespace pentago {
namespace mpi {

using Log::cout;
using std::endl;
using std::flush;
using std::tr1::unordered_map;

static void meaningless_helper(block_store_t* const self, const local_id_t local_id) {
  const event_t event = self->local_block_event(local_id);
  thread_time_t time(meaningless_kind,event);

  // Prepare
  const block_info_t& info = self->block_info(local_id);
  const auto base = block_size*Vector<int,4>(info.block);
  const auto shape = block_shape(info.section.shape(),info.block);
  const auto rmin0 = safe_rmin_slice(info.section.counts[0],base[0]+range(shape[0])),
             rmin1 = safe_rmin_slice(info.section.counts[1],base[1]+range(shape[1])),
             rmin2 = safe_rmin_slice(info.section.counts[2],base[2]+range(shape[2])),
             rmin3 = safe_rmin_slice(info.section.counts[3],base[3]+range(shape[3]));
#if PENTAGO_MPI_COMPRESS
  const auto flat_data = large_buffer<Vector<super_t,2>>(shape.product(),false);
#else
  const auto flat_data = self->all_data.slice(info.nodes);
  OTHER_ASSERT(shape.product()==flat_data.size());
#endif
  const RawArray<Vector<super_t,2>,4> block_data(shape,flat_data.data());

  // Fill and count
  Vector<uint64_t,3> counts;
  for (int i0=0;i0<shape[0];i0++)
    for (int i1=0;i1<shape[1];i1++)
      for (int i2=0;i2<shape[2];i2++)
        for (int i3=0;i3<shape[3];i3++) {
          auto& node = block_data(i0,i1,i2,i3);
          const auto board = quadrants(rmin0[i0],rmin1[i1],rmin2[i2],rmin3[i3]);
          // It is important that the probability of winning here is fairly high, since
          // otherwise the high branching factor means that essentially all positions one
          // level up will be a win.  All wins makes for a very boring analysis.
          node.x = super_meaningless(board)|super_meaningless(board,1)|super_meaningless(board,2)|super_meaningless(board,3);
          node.y = node.x|super_meaningless(board,7);
          // Count wins
          counts += Vector<uint64_t,3>(popcounts_over_stabilizers(board,node)); 
        }
  info.missing_dimensions = 0;

  // Sample
  for (auto& sample : self->samples[info.flat_id])
    sample.wins = block_data.flat[sample.index];

#if PENTAGO_MPI_COMPRESS
  time.stop();
  // Compress data into place
  self->store.compress_and_set(info.flat_id,flat_data,event);
#endif

  // Add to section counts
  const int section_id = self->sections->section_id.get(info.section);
  spin_t spin(self->section_counts_lock);
  self->section_counts[section_id] += counts;
}

Ref<block_store_t> meaningless_block_store(const partition_t& partition, const int rank, const int samples_per_section) {
  Log::Scope scope("meaningless");

  // Allocate block store
  const auto self = make_block_store(partition,rank,samples_per_section);

  // Replace data with meaninglessness
  memset(self->section_counts.data(),0,sizeof(Vector<uint64_t,3>)*self->section_counts.size());
  for (const auto& info : self->block_infos)
    threads_schedule(CPU,curry(meaningless_helper,&*self,info.key));
  threads_wait_all_help();
  return self;
}

static Vector<uint64_t,3> meaningless_counts(RawArray<const board_t> boards) {
  Vector<uint64_t,3> counts; 
  counts.z = boards.size();
  for (const auto board : boards) {
    const bool x = meaningless(board)|meaningless(board,1)|meaningless(board,2)|meaningless(board,3),
               y = x|meaningless(board,7);
    counts.x += x;
    counts.y += y;
  }
  return counts;
}

// Map a board to its index within the section that owns it, asserting that the board is rotation standardized.
static Vector<int,4> standard_board_index(board_t board) {
  Vector<int,4> index;
  for (int a=0;a<4;a++) {
    const int ir = rotation_minimal_quadrants_inverse[quadrant(board,a)];
    OTHER_ASSERT(!(ir&3));
    index[a] = ir>>2;
  }
  return index;
}

// All sparse samples must occur in this block store
static void compare_blocks_with_sparse_samples(const block_store_t& blocks, RawArray<const board_t> boards, RawArray<const Vector<super_t,2>> data) {
  OTHER_ASSERT(boards.size()==data.size());
  OTHER_ASSERT(blocks.partition->ranks==1);
 
  // Partition samples by block.  This is necessary to avoid repeatedly decompressing compressed blocks.
  Hashtable<local_id_t,Array<Tuple<int,Vector<int,4>>>> block_samples;
  for (int i=0;i<boards.size();i++) {
    const auto board = boards[i];
    check_board(board);
    const auto section = count(board);
    const auto index = standard_board_index(board);
    const Vector<uint8_t,4> block(index/block_size);
    block_samples.get_or_insert(blocks.block_to_local_id.get(tuple(section,block))).append(tuple(i,index-block_size*Vector<int,4>(block)));
  }

  // Check all samples
  for (auto& samples : block_samples) {
    const auto local_id = samples.key;
    const bool turn = blocks.block_info(local_id).section.sum()&1;
#if PENTAGO_MPI_COMPRESS
    const auto block_data = blocks.uncompress_and_get(local_id,unevent);
#else
    const auto block_data = blocks.get_raw(local_id);
#endif
    for (const auto& sample : block_samples[local_id]) {
      OTHER_ASSERT(block_data.valid(sample.y));
      auto entry = block_data[sample.y];
      entry.y = ~entry.y;
      if (turn)
        swap(entry.x,entry.y);
      OTHER_ASSERT(entry==data[sample.x]);
    }
  }
}

static void compare_blocks_with_supertensors(const block_store_t& blocks, const vector<Ref<supertensor_reader_t>>& readers) {
  // Verify that all blocks in the readers occur in our block store
  int count = 0;
  for (const auto& reader : readers) {
    const auto section = reader->header.section;
    const Vector<int,4> shape(reader->header.blocks);
    count += shape.product();
    for (int i0 : range(shape[0]))
      for (int i1 : range(shape[1]))
        for (int i2 : range(shape[2]))
          for (int i3 : range(shape[3]))
            blocks.assert_contains(section,vec<uint8_t>(i0,i1,i2,i3));
  }
  OTHER_ASSERT(blocks.total_blocks()==count);

  // Verify that all local blocks occur in the readers, and that all data matches
  unordered_map<section_t,int,Hasher> reader_id;
  for (int i=0;i<(int)readers.size();i++)
    reader_id[readers[i]->header.section] = i;
  for (const auto& info : blocks.block_infos) {
    const auto local_id = info.key;
    // Check that this block exists in the readers
    const auto it = reader_id.find(info.data.section);
    OTHER_ASSERT(it != reader_id.end());
    const auto& reader = *readers[it->second];

    // Verify that all data matches
    const auto read_data = reader.read_block(info.data.block).flat;
#if PENTAGO_MPI_COMPRESS
    const auto good_data = blocks.uncompress_and_get_flat(local_id,unevent);
#else
    const auto good_data = blocks.get_raw_flat(local_id);
#endif
    OTHER_ASSERT(read_data.size()==good_data.size());
    const bool turn = info.data.section.sum()&1;
    for (int i=0;i<read_data.size();i++) {
      auto good = good_data[i];
      good.y = ~good.y;
      if (turn)
        swap(good.x,good.y);
      OTHER_ASSERT(good==read_data[i]);
    }
  }
}

namespace {
struct subcompare_t {
  Vector<uint8_t,4> block;
  spinlock_t lock;
  Array<const Vector<super_t,2>,4> old_data;
};

struct compare_t : public boost::noncopyable {
  const section_t section;
  const Ptr<const supertensor_reader_t> old_reader;
  vector<Array<Tuple<Vector<int,4>,Vector<super_t,2>>>> block_samples;
  spinlock_t lock;
  ProgressIndicator progress;
  Vector<uint64_t,3> counts;
  int checked;

  compare_t(const section_t section, const Ptr<const supertensor_reader_t> old_reader)
    : section(section)
    , old_reader(old_reader)
    , progress(section_blocks(section).product(),true)
    , checked(0) {}
};
}

static void process_data(compare_t* self, subcompare_t* sub, RawArray<const Tuple<Vector<int,4>,Vector<super_t,2>>> samples, Vector<uint8_t,4> block, Array<const Vector<super_t,2>,4> data) {
  OTHER_ASSERT(block==sub->block);
  sub->lock.lock();
  if (!sub->old_data.flat.size() && self->old_reader) {
    sub->old_data = data;
    sub->lock.unlock();
  } else {
    // Compare with old data if it exists
    if (self->old_reader) {
      OTHER_ASSERT(sub->old_data.shape==data.shape);
      OTHER_ASSERT(sub->old_data==data);
    }
    // Compare samples
    for (const auto& sample : samples) {
      OTHER_ASSERT(data.valid(sample.x));
      OTHER_ASSERT(data[sample.x]==sample.y);
    }
    // Count
    const auto counts = count_block_wins(self->section,block,data.flat);
    {
      spin_t spin(self->lock);
      self->counts += counts;
      self->progress.progress();
      self->checked++;
    }
    // Done with this block
    delete sub;
  }
}

// Check one supertensor file against another, and verify that all sparse samples are consistent.  Return counts and number of samples found in this section.
static Tuple<Vector<uint64_t,3>,int> compare_readers_and_samples(const supertensor_reader_t& reader, const Ptr<const supertensor_reader_t> old_reader, RawArray<const board_t> sample_boards, RawArray<const Vector<super_t,2>> sample_wins) {
  compare_t self(reader.header.section,old_reader);
  OTHER_ASSERT((int)reader.header.block_size==block_size);
  OTHER_ASSERT(sample_boards.size()==sample_wins.size());
  if (old_reader) {
    OTHER_ASSERT(old_reader->header.section==self.section);
    OTHER_ASSERT((int)old_reader->header.block_size==block_size);
  }

  // Organize samples by block
  const Vector<int,4> blocks(reader.header.blocks);
  vector<Array<Tuple<Vector<int,4>,Vector<super_t,2>>>> block_samples(blocks.product());
  int sample_count = 0;
  for (int i=0;i<sample_boards.size();i++) {
    const auto board = sample_boards[i];
    const auto sample_section = count(board);
    if (self.section==sample_section) {
      const auto index = standard_board_index(board);
      const auto block = index/block_size;
      block_samples.at(pentago::index(blocks,block)).append(tuple(index-block_size*block,sample_wins[i]));
      sample_count++;
    }
  }

  // Perform checking in parallel
  cout << self.section << ' ' << flush;
  for (int i0 : range(blocks[0]))
    for (int i1 : range(blocks[1]))
      for (int i2 : range(blocks[2]))
        for (int i3 : range(blocks[3])) {
          auto sub = new subcompare_t;
          const auto block = sub->block = vec<uint8_t>(i0,i1,i2,i3);
          const function<void(Vector<uint8_t,4>,Array<Vector<super_t,2>,4>)> process = curry(process_data,&self,sub,block_samples.at(index(blocks,Vector<int,4>(block))).raw());
          reader.schedule_read_block(block,process);
          if (old_reader)
            old_reader->schedule_read_block(block,process);
        }
  threads_wait_all();
  OTHER_ASSERT(self.checked==blocks.product());
  return tuple(self.counts,sample_count);
}

}
}
using namespace pentago::mpi;

void wrap_check() {
  OTHER_FUNCTION(meaningless_block_store)
  OTHER_FUNCTION(meaningless_counts)
  OTHER_FUNCTION(compare_blocks_with_sparse_samples)
  OTHER_FUNCTION(compare_blocks_with_supertensors)
  OTHER_FUNCTION(compare_readers_and_samples)
}
