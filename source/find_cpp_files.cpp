#include "find_cpp_files.hpp"
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/filesystem.hpp>

namespace cell
{

namespace
{

bool is_cpp(const boost::filesystem::directory_entry& e) { return e.path().extension() == ".cpp"; }
std::string to_string(const boost::filesystem::directory_entry& e) { return e.path().string(); }
}

paths find_cpp_files()
{
    using namespace boost::filesystem;
    using namespace boost::range;
    using namespace boost::adaptors;
    decltype(find_cpp_files()) cpps;
    push_back(cpps, transform(filter(make_iterator_range(directory_iterator(current_path()), directory_iterator()), is_cpp), to_string));
    return cpps;
}

}
