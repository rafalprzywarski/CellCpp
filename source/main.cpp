#include <iostream>
#include <fstream>

int main()
{
    std::ifstream project("build.cell");
    if (!project.is_open())
    {
        std::cerr << "build.cell not found" << std::endl;
        return 1;
    }

    std::string dummy, project_name;
    project >> dummy >> project_name;

    auto cmd = "g++ *.cpp -o " + project_name;

    std::system(cmd.c_str());

    return 0;
}
