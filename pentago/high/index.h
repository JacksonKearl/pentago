// Uncompressed index for supertensor files
#pragma once

/* A supertensor index file (.pentago.index) contains uncompressed index information
 * for a supertensor file suitable for uncompressed random access.  The format is
 *
 *   char magic[20] = "pentago index      \n";
 *   uint32_t slice; // .pentago.index files always contain a complete slice
 *   compact_blob_t blobs[section_id][block]; // compact blobs in standard order
 *
 * where compact_blob_t packs offset and compressed_size into 12 bytes as defined below,
 * and the section order is as computed by descendent_sections in pentago/end/sections.h.
 * All data is little endian.
 */

#include <pentago/end/sections.h>
#include <pentago/data/supertensor.h>
#include <boost/detail/endian.hpp>
#ifdef BOOST_LITTLE_ENDIAN
namespace pentago {

struct __attribute__ ((packed)) compact_blob_t {
  uint64_t offset;
  uint32_t size; // compressed for blocks, uncompressed for blob information
};
static_assert(sizeof(compact_blob_t)==12,"struct packing failed");

struct supertensor_index_t : public Object {
  GEODE_DECLARE_TYPE(GEODE_EXPORT)
  typedef Tuple<section_t,Vector<uint8_t,4>> block_t;

  const Ref<const end::sections_t> sections;
  const Array<const uint64_t> section_offset;

protected:
  supertensor_index_t(const end::sections_t& sections);
public:
  ~supertensor_index_t();

  string header() const;
  compact_blob_t blob_location(const block_t block) const;

  // Convert blob data to block location
  static compact_blob_t block_location(RawArray<const uint8_t> blob);

  // Decompress a compressed block
  static Array<Vector<super_t,2>,4> unpack_block(const block_t block, RawArray<const uint8_t> compressed);
};

void write_supertensor_index(const string& name, const vector<Ref<const supertensor_reader_t>>& readers);

}
#endif