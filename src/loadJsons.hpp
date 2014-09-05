#include <string>
#include <vector>
#include <iterator>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/range/iterator_range.hpp>

using namespace std;
using namespace boost::filesystem;
using boost::property_tree::ptree;

namespace dfg {
  const string jsonExtension = ".json";

  inline vector<ptree> loadJsons(const string &pathName) {
    vector<ptree> jsons;
    path rootPath(pathName);
    if(!exists(rootPath)) {
      throw "No such path";
    }
    if(!is_directory(rootPath)) {
      throw "Must call loadJsons with a directory";
    }

    for(auto entry : boost::make_iterator_range(recursive_directory_iterator(rootPath),{})) {
      const path &p = entry.path();
      if(is_regular_file(p) && extension(p) == jsonExtension) {
        ptree json; 
        boost::filesystem::ifstream stream(p);
        read_json(stream,json);
        jsons.push_back(move(json));
      }
    }

    return jsons;
  }
}


