// Spin locks
#pragma once

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#else
#include <pthread.h>
#endif
#include <boost/noncopyable.hpp>
namespace pentago {

/* Important non-portability note:
 *
 * The whole point of a spin lock is to be cheap and compact.  Thus, a spin lock is probably
 * an int, and the idea of a spin lock having a nontrivial destructor seems silly.  Therefore,
 * our spinlock_t will have a trivial destructor.  If we use pthreads for spinlocks and the
 * destructor does something complicated, we're going to leak a bunch of spinlocks.  I can't
 * imagine this actually happening, but just to be safe I'm going to require GLIBC for now in
 * the pthreads case.
 */

// Apple has their own special version, which I actually like more than the pthreads version.
// In particular, they guarantee that a spinlock is an integer, and zero means unlocked.
#if defined(__APPLE__)

struct spinlock_t {
  OSSpinLock spinlock;

  spinlock_t()
    : spinlock(0) {}

  void lock() {
    OSSpinLockLock(&spinlock);
  }

  void unlock() {
    OSSpinLockUnlock(&spinlock);
  }
};

// Use pthreads if we have them.
#elif defined(_POSIX_SPIN_LOCKS) && _POSIX_SPIN_LOCKS>0

// Warning: We explicitly break the pthread standard in two ways.
//
// 1. We assume pthread_spin_destroy is a no-op.
// 2. We ignore all error codes.
//
// I've checked that both are safe to do with glibc, so we restrict to that for now.
#ifdef __GLIBC__
BOOST_STATIC_ASSERT(boost::is_same<volatile int,pthread_spinlock_t);
#else
#error "I'm not sure if pthread_spin_destroy is a no-op on this system"
#endif

struct spinlock_t {
  pthread_spinlock_t spinlock;

  spinlock_t() {
    // We use MPI for interprocess communication, so private is enough
    pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);
  }

  // Explicitly don't use a copy constructor.

  void lock() {
    pthread_spin_lock(&spinlock);
  }

  void unlock() {
    pthread_spin_unlock(&spinlock);
  }
};

#else
#error "No available spin lock implementation"
#endif

// Convenience class to lock and unlock a spinlock
struct spin_t : boost::noncopyable {
  spinlock_t& lock;

  spin_t(spinlock_t& lock)
    : lock(lock) {
    lock.lock();
  }

  ~spin_t() {
    lock.unlock();
  }
};

}