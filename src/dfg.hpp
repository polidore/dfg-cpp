#ifndef DFG_HPP
#define DFG_HPP

#include <unordered_map>
#include <map>
#include <utility>
#include <forward_list>
#include <vector>
#include <string>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

#include "loadJsons.hpp"

using namespace std;
using boost::property_tree::ptree;

namespace dfg {
  enum CacheStatus { Primary,Secondary,None };

  class DFGType {
    public:
      DFGType(string typeName, forward_list<ptree> typeFragments, vector<string> overrideScheme);
      shared_ptr<ptree> getCfg(const map<string,string>& context);
      const inline string getTypeName() { return _typeName; };
      const inline forward_list<ptree>& getFragments() { return _fragments; };
      const inline vector<string>& getOverrideScheme() { return _overrideScheme; };
      const inline CacheStatus getLastCacheStatus() { return _lastCacheStatus; };
    private:
      bool checkCollision();
      ptree mergeCfgs(const forward_list<shared_ptr<ptree>>& fragments);
      tuple<string,forward_list<shared_ptr<ptree>>> match(const map<string,string>& context);
      bool fragMatch(const ptree& fragment,const map<string,string>& context);
      string makeHash(const map<string,string>& context);
    private:
      string _typeName;
      forward_list<ptree> _fragments;
      vector<string> _overrideScheme;
      unordered_map<string,ptree> _primaryCache;
      unordered_map<string,forward_list<shared_ptr<ptree>>> _secondaryCache;
      CacheStatus _lastCacheStatus;
  };

  class DFGTypeFactory {
    public:
      DFGTypeFactory(string path);
      const shared_ptr<DFGType> createType(string typeName, vector<string> overrideScheme);
      const inline shared_ptr<DFGType> getType(string typeName) { return _types[typeName]; };
    private:
      forward_list<ptree> _fragments;
      unordered_map<string,shared_ptr<DFGType>> _types;
  };
  
  DFGTypeFactory::DFGTypeFactory(string path) {
    _fragments = loadJsons(path);
  }

  //Type
  /////////////////////////
  DFGType::DFGType(string typeName, forward_list<ptree> typeFragments, vector<string> overrideScheme) {
    _typeName = typeName;
    _fragments = typeFragments;
    _overrideScheme = overrideScheme;
    _lastCacheStatus = CacheStatus::None;
  }

  //Type Factory
  ////////////////////////
  const shared_ptr<DFGType> DFGTypeFactory::createType(string typeName, vector<string> overrideScheme) {
    forward_list<ptree> typeFragments;

    auto prevIt = _fragments.before_begin();
    for(auto it = _fragments.begin(); it != _fragments.end(); ) {
      auto fragment = *it;
      if(fragment.get<string>("@type") == typeName) {
        typeFragments.push_front(std::move(fragment));
        it = _fragments.erase_after(prevIt);
      }
      else {
        prevIt = it;
        ++it;
      }
    }

    auto type = make_shared<DFGType>(typeName,move(typeFragments),overrideScheme);
    _types.insert(make_pair(typeName,type));
    return type;
  }
}

#endif
