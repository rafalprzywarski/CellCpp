Feature: As a developer I want the build system to build my programs

    @done
    Scenario: No project file
        When I run cell
        Then it should fail with "build.cell not found"
