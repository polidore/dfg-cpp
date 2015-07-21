#include <unordered_map>
#include <string>
#include <boost/property_tree/ptree.hpp>

#include "loadJsons.hpp"

using namespace std;
using namespace boost::property_tree::ptree;

namespace dfg {
  class DFGFactory {
    public:
      DFGFactory() {
      }
      ~DFGFactory() {
      }

    private:
      unordered_map<string,DFGType> _types;
  }

  class DFGType {
    public:
      DFGType() {
    }
    public:
      ~DFGType() {
    }

    private:
      vector<ptree> _fragments;
      unordered_map<string,ptree> _primaryCache;
      unordered_map<string,ptree> _secondaryCache;
  }
}
