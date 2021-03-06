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
        And files "build/test1.cpp.o build/test2.cpp.o build/test_main.cpp.o" should exist
        And compiled program "build/demo" should print:
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
        Then only files "build/file2.cpp.d build/main.cpp.d build/file2.cpp.o build/main.cpp.o build/test37" should change

    @done
    Scenario: Should use executable name if one is preset in the configuration
        Given project:
        """
        project: some
        executable: proj_exe
        """
        And main source "some.cpp"
        When I run cell
        Then file "build/proj_exe" should exist
        And file "build/some" should not exist

    @done
    Scenario: Do recompile source files if their included header files change
      Given project "test37" with main source "main.cpp" and sources "file1.cpp file2.cpp file3.cpp"
      And file "file4.h" with:
      """
      void f4();
      """
      And file "file4.cpp" with:
      """
      #include "file4.h"
      void f4() { }
      """
      When I run cell
      And I touch "file4.h"
      And I keep timestamps of all files
      And I run cell
      Then only files "build/file4.cpp.o build/file4.cpp.d build/test37" should change

    @done
    Scenario: Should rebuild a header list if its .cpp changes
        Given project "test22" with main source "main.cpp"
        And file "file4.h" with:
        """
        void f4();
        """
        And file "file4.cpp" with:
        """
        void f4() { }
        """
        When I run cell
        Given file "file4.cpp" with:
        """
        #include "file4.h"
        void f4() { }
        """
        When I run cell
        And I touch "file4.h"
        And I keep timestamps of all files
        And I run cell
        Then only files "build/file4.cpp.o build/file4.cpp.d build/test22" should change

    @done
    Scenario: Should rebuild a header list if on of the headers change
        Given project "test22" with main source "main.cpp"
        And file "file4.h" with:
        """
        void f4();
        """
        And file "file5.h" with:
        """
        void f5();
        """
        Given file "file4.cpp" with:
        """
        #include "file4.h"
        void f4() { }
        """
        When I run cell
        Given file "file4.h" with:
        """
        #include "file5.h"
        void f4();
        """
        When I run cell
        And I touch "file5.h"
        And I keep timestamps of all files
        And I run cell
        Then only files "build/file4.cpp.o build/file4.cpp.d build/test22" should change

    @done
    Scenario: Should put all generated files in the specified output path
        Given project:
        """
        project: some
        output-path: 'out/debug'
        """
        And main source "some.cpp"
        When I run cell
        Then files "out/debug/some.cpp.d out/debug/some.cpp.o out/debug/some" should exist
