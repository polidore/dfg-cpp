#include "catch.hpp"
#include "../src/loadJsons.hpp"
#include "../src/dfg.hpp"

TEST_CASE("Should load basic json data", "[json]") { 
  auto jsons = dfg::loadJsons("./");
  REQUIRE(std::distance(jsons.begin(),jsons.end()) == 3);
  int numOverride = 0;
  int numVoltage = 0;
  for(auto j : jsons) {
    REQUIRE(j.get<std::string>("@type") == "electricity");
    numOverride += j.count("@override");
    numVoltage += j.count("voltage");
  }
  REQUIRE(numOverride == 2);
  REQUIRE(numVoltage == 2);
}

TEST_CASE("Create a factory", "[dfg]") {
  auto factory = dfg::DFGTypeFactory("./");
  auto type = factory.createType("electricity",{"country","state"});
  REQUIRE(type.use_count() == 2);
  auto type2 = factory.getType("electricity");
  REQUIRE(type == type2);
  REQUIRE(type2.use_count() == 3);
  REQUIRE(type.use_count() == 3);
}

TEST_CASE("Create a valid type","[dfg]") {
  auto factory = dfg::DFGTypeFactory("./");
  auto type = factory.createType("electricity",{"country","state"});
  REQUIRE(type->getLastCacheStatus() == dfg::CacheStatus::None);
  REQUIRE(type->getTypeName() == "electricity");
  auto fragments = type->getFragments();
  REQUIRE(std::distance(fragments.begin(),fragments.end()) == 3);
  REQUIRE(type->getOverrideScheme().size() == 2);

  REQUIRE(type.use_count() == 2);
}
