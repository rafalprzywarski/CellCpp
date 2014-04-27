#ifndef CELL_17F97883E8F04AEDAEE29C4CF92F2260_HPP
#define CELL_17F97883E8F04AEDAEE29C4CF92F2260_HPP
#include <string>
#include <vector>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>

namespace cell
{
namespace config
{
struct property
{
    std::string name, value;
};

typedef std::vector<property> properties;

}
}

BOOST_FUSION_ADAPT_STRUCT(
    cell::config::property,
    (std::string, name)
    (std::string, value)
)
#endif
