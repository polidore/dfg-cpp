dfg-cpp
=======

C++ Implementation of DFG.  See the nodejs [implementation](https://github.com/polidore/dfg) for more documentation. 

## Design

* For each file path, there will be a static struct of:
  1. vector of raw json files
  1. primary cache
  1. secondary cache
* users will stack allocate a handle to the configs that will point to a one of the above based on the path used


So, it's not really a singleton, but it manages the memory allocation based on a path that defines a unique key
