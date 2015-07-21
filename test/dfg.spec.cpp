#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/loadJsons.hpp"

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
