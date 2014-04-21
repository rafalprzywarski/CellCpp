Feature: As a developer I want the build system to build my programs

    @done
    Scenario: No project file
        When I run cell
        Then it should fail with "build.cell not found"

    @done
    Scenario: Compile all .cpp's in current directory
        Given file "build.cell" with:
        """
        project: demo

        """
        And file "test1.cpp" with:
        """
        #include <iostream>
        void test1() { std::cout << "test1" << std::endl; }

        """
        And file "test2.cpp" with:
        """
        #include <iostream>
        void test2() { std::cout << "test2" << std::endl; }

        """
        And file "test_main.cpp" with:
        """
        #include <iostream>
        void test1();
        void test2();
        int main()
        {
            test1();
            test2();
            std::cout << "all done" << std::endl;
        }

        """
        When I run cell
        Then it should not fail
        And compiled program "demo" should print:
        """
        test1
        test2
        all done

        """
