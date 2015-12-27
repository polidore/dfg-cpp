dfg-cpp
=======

C++ Implementation of DFG.  See the nodejs [implementation](https://github.com/polidore/dfg) for more documentation.

## Building

* Requires boost
* use cmake to create build environment
* Tested on:
  * clang / make
  * Xcode
  * Visual Studio
  * gcc / make

```
cmake .
make
make test
```

## Basic Use

Consider this json config set:

```json
{
  "@type": "electricity",
  "kwhRate": 0.2,
  "ac": true,
  "voltage": 220,
  "hydro": 0.2
}

{
  "@type": "electricity",
  "@override": {
    "country": "US"
  },
  "voltage": 110,
  "kwhRate": 0.12
}
```

Use this C++ to get the US configs overridden on the defaults.

```c++
#include "dfg.hpp"

auto factory = dfg::DFGTypeFactory("./test/jsons");
auto type = factory.createType("electricity",{"country","state","county","city"});
map<string,string> context {
    {"country","us"}
};
auto cfg = type->getCfg(context);
cout << "US voltage: " << cfg->get<int>("voltage") << endl; //US voltage: 110
```
