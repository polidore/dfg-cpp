#include "catch.hpp"
#include "../src/loadJsons.hpp"
#include "../src/dfg.hpp"
#include "../src/util.hpp"
#include <map>

TEST_CASE("Should load basic json data", "[json]") {
  auto jsons = dfg::loadJsons("./test/jsons");
  REQUIRE(std::distance(jsons.begin(),jsons.end()) == 9);
}

TEST_CASE("DFG Basics", "[dfg]") {
  auto factory = dfg::DFGTypeFactory("./test/jsons");

  SECTION("Test the factory") {
    auto type = factory.createType("electricity",{"country","state","county","city"});
    REQUIRE(type.use_count() == 2);
    auto type2 = factory.getType("electricity");
    REQUIRE(type == type2);
    REQUIRE(type2.use_count() == 3);
    REQUIRE(type.use_count() == 3);
  }

  SECTION("Test Bad Type") {
    REQUIRE_THROWS_AS(auto type = factory.createType("electricity_bad",{"country","state","county","city"}),dfg::CollisionException);
  }

  SECTION("Test Electricity DFGType") {
    auto type = factory.createType("electricity",{"country","state","county","city"});
    REQUIRE(type->getLastCacheStatus() == dfg::CacheStatus::None);
    REQUIRE(type->getTypeName() == "electricity");
    auto fragments = type->getFragments();
    REQUIRE(std::distance(fragments.begin(),fragments.end()) == 4);
    REQUIRE(type->getOverrideScheme().size() == 4);
    REQUIRE(type.use_count() == 2);

    REQUIRE(dfg::hasKey<string>(*fragments.begin(),"@override") == false); //defaults
    REQUIRE(dfg::hasKey<string>(*(++fragments.begin()),"@override") == true);

    map<string,string> context {
        {"country","us"},
        {"state","NY"},
        {"county","new york"},
        {"city","new york"}
    };
    WARN("About to run getCfg");
    auto cfg = type->getCfg(context);
    WARN("Ran getcfg");
    REQUIRE(context.count("@hash") == 1);
  }
}
