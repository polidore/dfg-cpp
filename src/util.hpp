#ifndef DFG_UTIL_HPP
#define DFG_UTIL_HPP

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using boost::property_tree::ptree;

namespace dfg {
  template <class T>
  static bool hasKey(const ptree& p,string path) {
    return p.get_optional<T>(path) != boost::none;
  }

  static string toString(const ptree &p) {
    stringstream ss;
    write_json(ss,p);
    return ss.str();
  }
}

#endif
