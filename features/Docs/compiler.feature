Feature: As a developer I want specify the compiler to build my software
	@done
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
		executable: './fakecc'
		compile-source: 'COMPILE $(SOURCE) OUTPUT $(OBJECT)'
		link-executable: 'LINK $(OBJECTS) AS $(EXECUTABLE)'
		get-used-headers: 'HEADERS $(SOURCE)'
		"""
		And main source "main.cpp"
		And sources "file1.cpp file2.cpp"
		When I run cell
		Then fakecc should run with "HEADERS main.cpp"
		And fakecc should run with "COMPILE main.cpp OUTPUT main.cpp.o"
		And fakecc should run with "HEADERS file1.cpp"
		And fakecc should run with "COMPILE file1.cpp OUTPUT file1.cpp.o"
		And fakecc should run with "HEADERS file2.cpp"
		And fakecc should run with "COMPILE file2.cpp OUTPUT file2.cpp.o"
		And fakecc should link files "main.cpp.o file1.cpp.o file2.cpp.o" by "LINK (.*) AS demo"
