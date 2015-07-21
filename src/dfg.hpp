#include <unordered_map>
#include <map>
#include <forward_list>
#include <vector>
#include <string>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

#include "loadJsons.hpp"

using namespace std;
using namespace boost::property_tree::ptree;

namespace dfg {
  class DFGTypeFactory {
    public:
      DFGTypeFactory(string path);
      shared_ptr<DFGType> createType(string typeName, vector<string> overrideScheme);
      shared_ptr<DFGType> getType(string typeName);
    private:
      forward_list<ptree> _fragments;
      unordered_map<string,shared_ptr<DFGType>> _types;
  }

  class DFGType {
    public:
      DFGType(string typeName, vector<ptree> typeFragments, vector<string> overrideScheme);
      ptree getCfg(map<string,string> context);
    private:
      bool checkCollision();
      ptree mergeCfgs(vector<ptree> fragments);
      tuple<string,vector<ptree>> searchFragments(map<string,string> context);
      bool fragMatch(ptree fragment,map<string,string> context);
      string makeHash(map<string,string> context);
    private:
      vector<ptree> _fragments;
      unordered_map<string,ptree> _primaryCache;
      unordered_map<string,vector<ptree>> _secondaryCache;
  }

  DFGTypeFactory::DFGTypeFactory(string path) {
    _fragments = loadJsons(path);
  }

  DFGType DFGTypeFactory::createType(string typeName, vector<string> overrideScheme) {
    vector<ptree> typeFragments;
    for(auto f : this._fragments) {
    }
    _types
  }
}
