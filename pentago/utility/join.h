// C++ version of Python's join
#pragma once

#include <geode/utility/config.h>
#include <string>
namespace pentago {

template<class A> static inline string join(const string& sep, const A& list) {
  string result;
  bool first = true;
  for (const string& s : list) {
    if (!first)
      result += sep;
    first = false;
    result += s;
  }
  return result;
}

}
