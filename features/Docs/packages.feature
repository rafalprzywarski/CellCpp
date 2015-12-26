Feature: As a developer I want to put my compiler code into packages
    @wip
    Scenario: Build a header-only package
        Given file "build.cell" with:
        """
        project: test
        compiler: fakecc
        packages: simple1
        """
        And file "simple1/build.cell" with:
        """
        library: simple1
        """
        And file "simple1/include/header1.h" with:
        """
        inline void f1() {}
        """
        And file "simple1/include/header2.hpp" with:
        """
        inline void f2() {}
        """
        And a fake compiler "fakecc"
        When I run cell
        Then file "build/simple1/include/header1.h" should exist
        And file "build/simple1/include/header2.hpp" should exist
        And file "build/simple1/build.cell" should not exist
        And fakecc should not run
