// Include mpi.h without getting deprecated C++ bindings
#pragma once

#ifdef OMPI_MPI_H
#error "mpi.h" already included
#endif

#define OMPI_SKIP_MPICXX 1
#include <mpi.h>

// Whether to enable expensive consistency checking
#define PENTAGO_MPI_DEBUG 0

// Whether to enable expensive MPI tracing
#define PENTAGO_MPI_TRACING 0

// Whether or not to store blocks compressed
#define PENTAGO_MPI_COMPRESS 1

// Whether or not to send output blocks compressed
#define PENTAGO_MPI_COMPRESS_OUTPUTS 0

// If true, poll for requests using MPI_Testsome instead of blocking
// with MPI_Waitsome.  This allows use of MPI_THREAD_FUNNELED.
#define PENTAGO_MPI_FUNNEL 0

namespace pentago {

// We fix the block size at compile time for optimization reasons
const int block_size = 8;
const int block_shift = 3;

// Hopefully conservative estimate of snappy's compression ratio on our data
const double snappy_compression_estimate = .45;

// Whether or not to use interleave filtered to precondition snappy
const bool snappy_filter = true;

// How many copies to post of each wildcard Irecv (for requests, responses, and outputs).
// This significantly improves latency.
const int wildcard_recv_count = 8;

}
