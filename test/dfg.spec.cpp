#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/loadJsons.hpp"

TEST_CASE("Should load basic json data", "[json]") { 
  auto jsons = dfg::loadJsons("./");
  REQUIRE(jsons.size() == 3);
  REQUIRE(jsons[0].get<int>("voltage") == 110);
  int numOverride = 0;
  for(auto j : jsons) {
    REQUIRE(j.get<std::string>("@type") == "electricity");
    numOverride += j.count("@override");
  }
  REQUIRE(numOverride == 2);
}
