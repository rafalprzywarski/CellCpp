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
        And object files "test1.o test2.o test_main.o" should exist
        And compiled program "demo" should print:
        """
        test1
        test2
        all done

        """
    @done
    Scenario: Do not compile twice if nothing changed
        Given default project
        When I run cell
        And I keep timestamps of all files
        And I run cell
        Then timestamps should not change
    @done
    Scenario: Do recompile if original files change
        Given project "test37" with main source "main.cpp" and sources "file1.cpp file2.cpp file3.cpp"
        When I run cell
        And I touch "main.cpp"
        And I touch "file2.cpp"
        And I keep timestamps of all files
        And I run cell
        Then only files "file2.o main.o test37" should change
