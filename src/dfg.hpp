#ifndef DFG_HPP
#define DFG_HPP

#include <unordered_map>
#include <map>
#include <utility>
#include <forward_list>
#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <cmath>

#include "loadJsons.hpp"
#include "util.hpp"

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
      void checkType() const;
      bool fragCmp(const ptree &a, const ptree &b) const;
      const ptree mergeCfgs(const forward_list<ptree>& fragments);
      const pair<string,forward_list<shared_ptr<ptree>>> match(const map<string,string>& context);
      bool fragMatch(const ptree& fragment,const map<string,string>& context) const;
      const string makeHash(const map<string,string>& context);
    private:
      string _typeName;
      forward_list<ptree> _fragments;
      vector<string> _overrideScheme;
      unordered_map<string,shared_ptr<ptree>> _primaryCache;
      unordered_map<string,forward_list<ptree>> _secondaryCache;
      CacheStatus _lastCacheStatus;
  };

  class DFGTypeFactory {
    public:
      DFGTypeFactory(string path);
      const shared_ptr<DFGType> createType(const string typeName, const vector<string>& overrideScheme);
      const inline shared_ptr<DFGType> getType(const string typeName) { return _types[typeName]; };
    private:
      forward_list<ptree> _fragments;
      unordered_map<string,shared_ptr<DFGType>> _types;
  };

  class CollisionException : public exception {
    public:
      CollisionException(string message) { _message = message; };
      string str() { return _message; };
    private:
      string _message;
  };


  //Type
  /////////////////////////
  DFGType::DFGType(string typeName, forward_list<ptree> typeFragments, vector<string> overrideScheme) :
    _typeName(typeName), _fragments(typeFragments), _overrideScheme(overrideScheme), _lastCacheStatus(CacheStatus::None)
  {
    _fragments.sort([this](ptree &a,ptree &b) { return fragCmp(a,b);});
    checkType();
  }

  bool DFGType::fragCmp(const ptree &a, const ptree &b) const {
    int aSum = 0, bSum = 0;

    for(auto i = 0; i < _overrideScheme.size(); i++) {
      int inc = pow(2,i);
      auto k = _overrideScheme[i];
      if(hasKey<string>(a,"@override." + k)) {
        aSum |= inc;
      }
      if(hasKey<string>(b,"@override." + k)) {
        bSum |= inc;
      }
    }
    return aSum < bSum;
  }

  const ptree DFGType::mergeCfgs(const forward_list<ptree>& fragments) {
    _lastCacheStatus = CacheStatus::None;
    if(fragments.begin() == fragments.end()) {
      throw "Nothing to merge";
    }

    auto head = fragments.begin();
    ptree base(*head); //copy & stack allocate. Will move later
    for(auto cur = ++head; cur != fragments.end(); cur++) {
      for(auto& kv : *cur) {
        if(kv.second.empty()) {
          base.put(kv.first,kv.second.get_value<string>());
        }
        else {
          base.erase(kv.first);
          base.add_child(kv.first,kv.second);
        }
      }
    }
  }

  void DFGType::checkType() const {
    auto prev = _fragments.begin();

    for(auto cur = _fragments.begin(); cur != _fragments.end(); cur++) {
      if(cur == prev) {
        if(hasKey<string>(*prev,"@override")) {
          throw CollisionException("No defaults for type");
        }
        continue;
      }

      if(!hasKey<string>(*cur,"@override")) {
        throw CollisionException("Default collision for type");
      }

      if(prev == _fragments.begin()) {
        prev++;
        continue;
      }

      int numCurOverrides = 0;
      bool different = false;
      for(auto& kv : cur->get_child("@override")) {
        numCurOverrides++;
        if(prev->get_optional<string>("@override." + kv.first) != kv.second.get_value<string>()) {
          different = true;
          break;
        }
      }


      if(!different && numCurOverrides == prev->get_child("@override").size()) {
        throw CollisionException("Duplicate keys:\n" + toString(cur->get_child("@override")) + toString(prev->get_child("@override")));
      }
      prev++;
    }
  }


  //Type Factory
  ////////////////////////
  DFGTypeFactory::DFGTypeFactory(string path) {
    _fragments = loadJsons(path);
  }

  const shared_ptr<DFGType> DFGTypeFactory::createType(const string typeName, const vector<string>& overrideScheme) {
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
