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
#include <memory>

#include "loadJsons.hpp"
#include "util.hpp"

using namespace std;
using boost::property_tree::ptree;

namespace dfg {
    enum CacheStatus { Primary,Secondary,Miss,None };

    class DFGType {
        public:
            DFGType(string typeName, forward_list<ptree> typeFragments, vector<string> overrideScheme);
            shared_ptr<ptree> getCfg(map<string,string>& context);
            const inline string getTypeName() { return _typeName; };
            const inline forward_list<ptree>& getFragments() { return _fragments; };
            const inline vector<string>& getOverrideScheme() { return _overrideScheme; };
            const inline CacheStatus getLastCacheStatus() { return _lastCacheStatus; };
        private:
            void checkType() const;
            bool fragCmp(const ptree &a, const ptree &b) const; //sorter
            ptree mergeCfgs(const vector<ptree*>& fragments);
            pair<string,vector<ptree*>> match(const map<string,string>& context);
            bool fragMatch(const ptree& fragment,const map<string,string>& context) const;
            string makeHash(const map<string,string>& context) const;
            string makeHashString(size_t size) const;
            map<string,string> contextFromOverride(const ptree& override) const;
        private:
            string _typeName;
            forward_list<ptree> _fragments;
            vector<string> _overrideScheme;
            unordered_map<string,shared_ptr<ptree>> _primaryCache;
            unordered_map<string,shared_ptr<ptree>> _secondaryCache;
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

    shared_ptr<ptree> DFGType::getCfg(map<string,string>& context) {
        auto hash = context.find("@hash"); // iter to reduce number of lookups
        string hashStr;
        if(hash != context.end()) {
            hashStr = hash->second;
        }
        else {
            hashStr = makeHash(context);
            context.insert({"@hash",hashStr});
        }

        auto cfg = _primaryCache.find(hashStr);
        if(cfg != _primaryCache.end()) {
            _lastCacheStatus = CacheStatus::Primary;
            return cfg->second;
        }

        auto matches = match(context);
        cfg = _secondaryCache.find(matches.first);
        if(cfg != _secondaryCache.end()) {
            _lastCacheStatus = CacheStatus::Secondary;
            _primaryCache.insert({hashStr,cfg->second});
            return cfg->second;
        }

        auto newCfg = make_shared<ptree>(mergeCfgs(matches.second));
        _secondaryCache.insert({matches.first,newCfg});
        _primaryCache.insert({hashStr,newCfg});
        _lastCacheStatus = CacheStatus::Miss;
        return newCfg;
    }

    bool DFGType::fragCmp(const ptree &a, const ptree &b) const {
        int aSum = 0, bSum = 0;

        for(auto i = 0; i < _overrideScheme.size(); i++) {
            int inc = (int)pow(2,i);
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

    map<string,string> DFGType::contextFromOverride(const ptree& override) const {
        map<string,string> m;
        for(auto& kv : override) {
            m.insert(make_pair(kv.first,kv.second.get_value<string>()));
        }
        return m;
    }

    pair<string,vector<ptree*>> DFGType::match(const map<string,string>& context) {
        string reducedHash = makeHashString(context.size());
        reducedHash = "default|";
        vector<ptree*> matches;

        for(auto cur = _fragments.begin(); cur != _fragments.end(); cur++)
        {
            if(fragMatch(*cur,context)) {
                matches.push_back(&*cur);

                auto fragOverride = cur->get_child_optional("@override");
                if(fragOverride) {
                    reducedHash += makeHash(contextFromOverride(*fragOverride));
                }
            }
        }
        return make_pair(reducedHash,move(matches));
    }

    ptree DFGType::mergeCfgs(const vector<ptree*>& fragments) {
        if(fragments.size() == 0) {
            throw "Nothing to merge";
        }

        auto head = fragments.begin();
        ptree base(**head);
        for(auto cur = ++head; cur != fragments.end(); cur++) {
            for(auto& kv : **cur) {
                if(kv.second.empty()) {
                    base.put(kv.first,kv.second.get_value<string>());
                }
                else {
                    base.erase(kv.first);
                    base.add_child(kv.first,kv.second);
                }
            }
        }
        return base;
    }

    bool DFGType::fragMatch(const ptree& fragment,const map<string,string>& context) const {
        auto override = fragment.get_child_optional("@override");
        if(!override) {
            return true; //defaults
        }

        for(auto& kv : context) {
            auto contextKey = kv.first;
            auto contextValue = kv.second;
            auto fragOverrideValue = override->get_optional<string>(contextKey);

            if(fragOverrideValue && *fragOverrideValue != contextValue) { //missing in frag means match all contexts
                return false;
            }
        }

        return true;
    }

    string DFGType::makeHashString(size_t size) const {
        string hash;
        size_t desiredSize = 20*size; //assumes 20 characters for key and value on avg
        if(hash.capacity() < desiredSize) { //in case this varies by platform
            hash.reserve(desiredSize);
        }
        return hash;
    }

    string DFGType::makeHash(const map<string,string>& context) const {
        string hash = makeHashString(context.size());

        for(auto& kv : context) {
            auto k = kv.first;
            auto v = kv.second;
            if(v == "") {
                continue;
            }
            hash += k + "=" + v + "|";
        }

        hash.shrink_to_fit();
        return hash;
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
