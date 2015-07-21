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
      DFGType(string typeName, forward_list<ptree> typeFragments, vector<string> overrideScheme);
      shared_ptr<ptree> getCfg(const map<string,string>& context);
    private:
      bool checkCollision();
      ptree mergeCfgs(const forward_list<shared_ptr<ptree>>& fragments);
      tuple<string,forward_list<shared_ptr<ptree>>> match(const map<string,string>& context);
      bool fragMatch(const ptree& fragment,const map<string,string>& context);
      string makeHash(const map<string,string>& context);
    private:
      forward_list<ptree> _fragments;
      unordered_map<string,ptree> _primaryCache;
      unordered_map<string,forward_list<shared_ptr<ptree>>> _secondaryCache;
  }

  DFGTypeFactory::DFGTypeFactory(string path) {
    _fragments = loadJsons(path);
  }

  shared_ptr<DFGType> DFGTypeFactory::createType(string typeName, vector<string> overrideScheme) {
    forward_list<ptree> typeFragments;
    auto prevIt = _fragments.before_begin();
    for(auto it = _fragments.begin(); it != _fragments.end(); ++it) {
      auto fragment = *it;
      if(fragment.get<string>("@type") == typeName) {
        typeFragments.push_front(std::move(fragment));
        _fragments.erase_after(prevIt);
      }
      prevIt = it;
    }
    shared_ptr<DFGType> type = new DFGType(typeName,move(typeFragments),overrideScheme);
    _types.insert(make_pair<string,shared_ptr<DFGType>>(typeName,type));
    return type;
  }
}
