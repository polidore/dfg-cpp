#include "catch.hpp"
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include "../src/util.hpp"

using boost::property_tree::ptree;

TEST_CASE("ptree copying semantics","[playground]") {
  ptree p1,p2;
  p1.put("a",7);
  p1.put("b.d",0);
  p2.put("b.c",1);
  INFO("p1: " << dfg::toString(p1));
  INFO("p2: " << dfg::toString(p2));

  ptree base(p1);
  REQUIRE(p1 == base);
  base.erase("b");
  base.add_child("b",p2.get_child("b"));
  REQUIRE(p2 != base);
  WARN("base: " << dfg::toString(base));

  ptree inside;
  inside.put("test.t",1);
  std::vector<ptree*> v;
  v.push_back(&inside);
  inside.put("test.t",0);
  v[0]->put("test.u",7);
  REQUIRE(inside == *v[0]);
  WARN("outside: " << dfg::toString(inside) << "inside: " << dfg::toString(*v[0]));
}
