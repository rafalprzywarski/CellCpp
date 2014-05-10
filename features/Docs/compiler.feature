Feature: As a developer I want specify the compiler to build my software
	@wip
	Scenario: Use a given compiler
		Given project file with:
		"""
		project: demo
		compiler: fakecc
		"""
		And a fake compiler "fakecc"
		And file "fakecc.cell" with:
		"""
		compiler: FakeCC
		executable: fakecc
		commands {
			compile-source: COMPILE $(SOURCE) OUTPUT $(OBJECT)
			link-executable: LINK $(OBJECTS) AS $(EXECUTABLE)
		}
		"""
		And main source "main.cpp"
		And sources "file1.cpp file2.cpp"
		When I run cell
		Then fakecc should run with " COMPILE main.cpp OUTPUT main.o"
		And fakecc should run with " COMPILE file1.cpp OUTPUT file1.o"
		And fakecc should run with " COMPILE file2.cpp OUTPUT file2.o"
		And fakecc should run with " LINK main.o file1.o file2.o file3.o AS demo"
