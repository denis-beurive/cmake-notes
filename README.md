# Notes about CMAKE

### Toggle Makefile verbosity

```cmake
set(CMAKE_VERBOSE_MAKEFILE off)
```

### Compile with DEGUG support

```cmake
set(CMAKE_BUILD_TYPE Debug)
```

### Tell the linker to use static libraries

```cmake
set(CMAKE_LINK_SEARCH_START_STATIC on)
```

### Print messages

You can use the [CMakePrintHelpers module](https://cmake.org/cmake/help/latest/module/CMakePrintHelpers.html).

```cmake
include(CMakePrintHelpers)
set(LOCAL_SRC_DIRECTORY "${CMAKE_SOURCE_DIR}/src")
cmake_print_variables(LOCAL_SRC_DIRECTORY)
```

### The path to the top level of the build / source tree

Be aware of the difference between BUILD and SOURCE trees.

* If you want to reference an element that is involved _during the build process_ (ex: a source code, a script...), then you reference the **source tree** (thus, must use `CMAKE_SOURCE_DIR`).
* If you want to reference a _destination of the build process_ (ex: a directory where to put an executable or a library), then you reference the **build tree** (thus, must use `CMAKE_BINARY_DIR`).

* Source tree: `${CMAKE_SOURCE_DIR}`
* Build tree: `${CMAKE_BINARY_DIR}`

Examples:

```cmake
include(CMakePrintHelpers)
cmake_print_variables(CMAKE_SOURCE_DIR)
cmake_print_variables(CMAKE_BINARY_DIR)
```

Result:

```
-- CMAKE_SOURCE_DIR="C:/Users/denis/Documents/github/aes"
-- CMAKE_BINARY_DIR="C:/Users/denis/Documents/github/aes/cmake-build-debug"
```

### Define a list of files

```cmake
set(LIB_LOGGER_SRC
        src/file1.c
        src/file1.h
)

set(LIB_JSON_SRC
        src/file2.c
        src/file2.h
)

list(APPEND ALL_SRC ${LIB_LOGGER_SRC} ${LIB_JSON_SRC})

add_library(mylib STATIC ${ALL_SRC})
# ...
```

### Create a library that is only a collection of libraries

Use the keyword `INTERFACE`.

    add_library(collection INTERFACE)
    target_link_libraries(collection INTERFACE lib_name1 lib_name2 lib_name3)

### Test whether an environment variable is set ot not

```cmake
if (DEFINED ENV{ODPIC_INCLUDE_PATH})
...
endif()
```

> WARNING: WARNING: this is counter-intuitive, pay attention. We did not write `$ENV{ODPIC_INCLUDE_PATH}` (no `$`).

### Print an informative message

```cmake
message("message to print")
```

### Print en error message and exit

```cmake
message(FATAL_ERROR "error message")
```

### Configure the search path for the header files

```cmake
include_directories(/path/to/includes1 /path/to/includes1 ...)
```

### Configure the search path for the libraries

```cmake
link_directories(/path/to/libraries1 /path/to/libraries2 ...)
```

### Add -D define flags to the compilation of source files

```cmake
add_definitions(-DDEF1 -DDEF2 ...) 
```

### Set compiler flags

Set compiler flags globally:

```cmake
add_definitions(-Wall -Wuninitialized -Wmissing-include-dirs -Wextra -Wconversion -Werror -Wfatal-errors -Wformat)
```

But, very often, you need to set specific compiler flags per targets :

```cmake
target_compile_options(logger PRIVATE -Wall -Wuninitialized -Wmissing-include-dirs -Wextra -Wconversion -Wunused-parameter -Wfatal-errors -Wformat)
```

> See this good explanation for PUBLIC, PRIVATE and INTERFACE: https://leimao.github.io/blog/CMake-Public-Private-Interface/

### Test the operating system

```cmake
message("os:  ${CMAKE_SYSTEM}")
if (CMAKE_SYSTEM MATCHES "Linux.*")
	...
elseif(CMAKE_SYSTEM MATCHES "Darwin.*")
	...
else()
    message( FATAL_ERROR "Unsupported OS ${CMAKE_SYSTEM}" )
endif()
```

### Test the host name

```cmake
cmake_host_system_information(RESULT HOST_NAME QUERY HOSTNAME)
message("Hostname: ${HOST_NAME}")
if (${HOST_NAME} MATCHES "test_srv*")
    ...
else()
    ...
endif()
```

### Run a script given from the standard input

```bash
cmake -P /dev/stdin <<<'MESSAGE(${CMAKE_ROOT})'
```

> This trick can be used to quickly test an element of syntax.

More elaborared examples:

```bash
cmake -P /dev/stdin <<<cat<<'EOF'
IF(DEFINED ENV{MYVAR})
    MESSAGE(STATUS "MYVAR env seen: --[$ENV{MYVAR}]--")
ELSE()
    MESSAGE(STATUS "MYVAR env not seen")
ENDIF()
EOF
```

=> `-- MYVAR env not seen`

```bash
MYVAR=123 cmake -P /dev/stdin <<<cat<<'EOF'
IF(DEFINED ENV{MYVAR})
    MESSAGE(STATUS "MYVAR env seen: --[$ENV{MYVAR}]--")
ELSE()
    MESSAGE(STATUS "MYVAR env not seen")
ENDIF()
EOF
```

=> `-- MYVAR env seen: --[123]--`

### Test the presence of headers and libraries

#### Test the presence of a header files

Here, we look for the ChilKat header files:

```cmake
include(CheckIncludeFiles)

...

set(CMAKE_REQUIRED_INCLUDES $ENV{CHILKAT_INCLUDE_PATH})
CHECK_INCLUDE_FILES("wchar.h;C_CkByteData.h;C_CkPrivateKey.h;C_CkString.h;C_CkCert.h;C_CkCrypt2.h;C_CkStringArray.h;C_CkBinData.h" HEADERS_CHILKAT LANGUAGE C)
if(NOT HEADERS_CHILKAT)
    message(FATAL_ERROR "Chilkat header files not found. Please configure CHILKAT_INCLUDE_PATH.")
else()
    message("OK: ChilKat header files found")
endif()
```

In case you have trouble:

```bash
rm CMakeCache.txt
cmake --debug-trycompile .
cat CMakeFiles/CMakeError.log 
```

Another way to perform the test _on a single file_:

```cmake
# Check that all required header files are available.
# 1. if environment variables are set, then use them to find the header files.
# 2. otherwise, try to find the header files while following the default system paths.

message("### Check the availability of the header file \"dpi.h\"")
if (DEFINED ENV{ODPIC_INCLUDE_PATH})
    message("ODPIC_INCLUDE_PATH is set: $ENV{ODPIC_INCLUDE_PATH}. Check this location.")
    find_path(DIR_HEADER_ODPIC dpi.h HINTS ENV ODPIC_INCLUDE_PATH)
    if(NOT DIR_HEADER_ODPIC)
        message(FATAL_ERROR "Header file not found at the provided location ($ENV{ODPIC_INCLUDE_PATH})")
    endif()
    message("Header file found at this location!")
    include_directories($ENV{ODPIC_INCLUDE_PATH})
else()
    message("ODPIC_INCLUDE_PATH is not set. Try to use default system paths.")
    find_path(DIR_HEADER_ODPIC dpi.h)
    if(NOT DIR_HEADER_ODPIC)
        message(FATAL_ERROR "Header file definitively not found.")
    endif()
    message("Header file found at default system path configuration (${DIR_HEADER_ODPIC})")
endif()
message("")
```

#### Test the presence of libraries

Here, we look for the library `libchilkat-9.5.0.a`:

```cmake
find_library(LIB_CHILKAT chilkat-9.5.0 HINTS ENV CHILKAT_LIBRARY_PATH)
if(NOT LIB_CHILKAT)
    message(FATAL_ERROR "Chilkat library not found. Please configure CHILKAT_LIBRARY_PATH.")
else()
    message("OK: ChilKat library found")
endif()
```

In case you have trouble:

```bash
rm CMakeCache.txt
cmake --debug-trycompile .
cat CMakeFiles/CMakeError.log 
```

Another way to perform the test:

```cmake
# Check that all required libraries are available.
# 1. if environment variables are set, then use them to find the libraries.
# 2. otherwise, try to find the library while following the default system paths.

message("### Check the availability of the library \"libodpic\"")
if (DEFINED ENV{ODPIC_LIBRARY_PATH})
    message("ODPIC_LIBRARY_PATH is set: $ENV{ODPIC_LIBRARY_PATH}. Check this location.")
    find_library(DIR_LIB_ODPIC odpic HINTS ENV ODPIC_LIBRARY_PATH)
    if(NOT DIR_LIB_ODPIC)
        message(FATAL_ERROR "Library not found at the provided location ($ENV{ODPIC_LIBRARY_PATH})")
    endif()
    message("Library found at this location!")
    link_directories($ENV{ODPIC_LIBRARY_PATH})
else()
    message("ODPIC_LIBRARY_PATH is not set. Try to use default system paths.")
    find_library(DIR_LIB_ODPIC odpic)
    if(NOT DIR_LIB_ODPIC)
        message(FATAL_ERROR "Library definitively not found.")
    endif()
    message("Library found at default system path configuration (${DIR_LIB_ODPIC})")
endif()
message("")
```

### Declare the path to a static library

```cmake
add_library(libodpic STATIC IMPORTED)
set_target_properties(libodpic PROPERTIES IMPORTED_LOCATION /path/to/lib/libodpic.4.1.0.a)
set_target_properties(libodpic PROPERTIES INTERFACE_INCLUDE_DIRECTORIES /path/to/include/directory)
```

### Declare a target static library

```cmake
add_library(nameA STATIC sourceA1 sourceA2 ...) # => libnameA.a
add_library(nameB STATIC sourceB1 sourceB2 ...) # => libnameB.a

set(TARGETS nameA nameB)

set_target_properties(
        ${TARGETS}
        PROPERTIES
        COMPILE_FLAGS -Wall
        ARCHIVE_OUTPUT_DIRECTORY lib
        EXCLUDE_FROM_ALL off
        DEPENDS other_target)
```

> This will build 2 static libraries `lib/libnameA.a` and `lib/libnameB.a`.
>
> Please note the use of the property `ARCHIVE_OUTPUT_DIRECTORY`.

* `COMPILE_FLAGS`: add flags for the compilation.
* `ARCHIVE_OUTPUT_DIRECTORY`: specify the path to the directory where to create the targets (`libnameA.a`, `libnameB.a`).
* `EXCLUDE_FROM_ALL`: tell whether the target must be compiled when building _all_, or not.
* `DEPENDS`: specify a dependency. The target `other_target` is required to build the listed targets (`libnameA.a` and `libnameB.a`).

> You may replace the use of the property "`COMPILE_FLAGS`" by a call to "`target_compile_options`".

### Create a static library with relocatable code

You have to set the property `POSITION_INDEPENDENT_CODE`. For example:

```cmake
add_library(string STATIC src/lib/libstring.c src/lib/libstring.h)
set_target_properties(
        string
        PROPERTIES
        COMPILE_FLAGS -Wall
        POSITION_INDEPENDENT_CODE ON
        ARCHIVE_OUTPUT_DIRECTORY lib
        EXCLUDE_FROM_ALL off)
```

> This is equivalent to the option `-fPic`.

> You may replace the use of the property "`COMPILE_FLAGS`" by a call to "`target_compile_options`".

### Create a shared library

Use the keyword `SHARED`. For example:

```cmake
add_library(parser001 SHARED src/lib/parsers/parser001.c src/lib/parsers/parser.h)
add_dependencies(parser001 string)
target_link_libraries(parser001 string pcre2-8)
set_target_properties(
        parser001
        PROPERTIES
        COMPILE_FLAGS -Wall
        POSITION_INDEPENDENT_CODE ON
        LIBRARY_OUTPUT_DIRECTORY parsers
        EXCLUDE_FROM_ALL off)
```

> You may replace the use of the property "`COMPILE_FLAGS`" by a call to "`target_compile_options`".

### Declare a target executable

```cmake
# Some dependencies...
add_library(dependency1 STATIC ...) # => libdependency1.a
add_library(dependency2 STATIC ...) # => libdependency2.a
add_library(dependency3 STATIC ...) # => libdependency3.a

...

# Declare executables
add_executable(executableA sourceA1 sourceA2 ...)
add_dependencies(executableA dependency1 dependency2)
target_link_libraries(executableA
    curl
    dependency1
    dependency2)

add_executable(executableB sourceB1 sourceB2 ...)
add_dependencies(executableB dependency1)
target_link_libraries(executableB
	zmq
	dependency1)

set(TEST_EXE executableA executableB)

set_target_properties(
        ${TEST_EXE}
        PROPERTIES
        COMPILE_FLAGS  -Wall
        RUNTIME_OUTPUT_DIRECTORY bin
        EXCLUDE_FROM_ALL off
        DEPENDS dependency3)
```


> This will build two executables: `bin/executableA` and `bin/executableB`).

* `COMPILE_FLAGS`: add flags for the compilation.
* `RUNTIME_OUTPUT_DIRECTORY`: specify the path to the directory where to create the targets (`executableA`, `executableB`).
* `EXCLUDE_FROM_ALL`: tell whether the target must be compiled when building _all_, or not.
* `DEPENDS`: specify a dependency. The target `dependency3` is required to build the listed targets (`executableA` and `executableB`).

> You may replace the use of the property "`COMPILE_FLAGS`" by a call to "`target_compile_options`".

Please note the use of the bloc below:

```cmake
target_link_libraries(executableA
    curl
    dependency1
    dependency2)
```

* We link the executable with the "external" Curl library.
* We link the executable with the libraries that were previously compiled (`dependency1` and `dependency2`).

### Define the "test" rule

```cmake
enable_testing()

set(LOCAL_TESTS_SCRIPTS_DIRECTORY "${CMAKE_SOURCE_DIR}/tests/script")
set(LOCAL_TESTS_BIN_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin")

...

# Build the programs that implement the tests
add_executable(test_program1 ...)
add_executable(test_program2 ...)

...

# Create the list of programs to execute in order to run the tests suite.
add_test(test_init      ${LOCAL_TESTS_SCRIPTS_DIRECTORY}/unit-tests-init.sh)
add_test(test_program1  ${LOCAL_TESTS_BIN_DIRECTORY}/test_program1)
add_test(test_program2  ${LOCAL_TESTS_BIN_DIRECTORY}/test_program2)
add_test(test_terminate ${LOCAL_TESTS_SCRIPTS_DIRECTORY}/unit-tests-terminate.sh)
```

And, you may want to set environment variables for the tests:

```cmake
set_tests_properties(
        test_init test_program1 test_program2 test_terminate
        PROPERTIES
        ENVIRONMENT "REPORT_DIR=${LOCAL_TESTS_REPORT_DIRECTORY};DATA_DIR=${LOCAL_TESTS_DATA_DIRECTORY};WORKBENCH_DIR=${LOCAL_TESTS_WORKBENCH_DIR_DIRECTORY}")
```

To execute the tests from the command line:

```cmake
cmake . && make && make test
```

or:

	ctest [--verbose]

### Customize the "clean" rule

```cmake
set_directory_properties(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "tests/bin/*;lib/*.a;bin/*")
```

### Add a custom target / rule

```cmake
add_custom_target(doc
	COMMAND bash -c "rm -rf doc/*"
	COMMAND bash -c "doxygen")
```

### Create a source file at build time

#### You want the source file to be created unconditionally (even if it already exists)

Scenario: you want to insert the date of the compilation into the executable you are building.

One solution is to produce a header file that defines a constant which represents the date.
For example, we can think of the header file `src/data.h`:

```c
#ifndef DATE
#define DATE "2020-6-11 10:11:22 0000 (UTC)"
#endif
```

This header file gets included in all executables sources. And these codes use the constant `DATE`.

To produce the header `src/date.h` we use a programme.
For example, we can think of the program `src/date.c`:

```c
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (2 != argc) {
        printf("Usage: %s <output file>\n", argv[0]);
        return 1;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    FILE *fd = fopen(argv[1], "w");
    if (NULL == fd) {
        fprintf(stderr, "Cannot open the file <%s> for writing.\n", argv[1]);
        return 1;
    }
    fprintf(fd, "#ifndef DATE\n");
    fprintf(fd, "#define DATE \"%d-%02d-%02d %02d:%02d:%02d %04ld (%s)\"\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            tm.tm_gmtoff,
            tm.tm_zone);
    fprintf(fd, "#endif\n");
    fclose(fd);
    return 0;
}
```

So, we need to:

* Compile `src/data.c` into `bin/date.exe`.
* Run `bin/date.exe src/date.h`.
* Compile all the executables that include `date.h`.

Declare the target `data.exe`. This will produce `date.exe` from `src/data.c`.

```cmake
add_executable(date.exe src/date.c)
set_target_properties(
        date.exe
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY bin)
```

Define a target name "date" (`make <target>`) that only runs the command that creates the header file `src/date.h`.

```cmake
add_custom_target(date
        COMMAND bin/date.exe src/date.h
        COMMENT "Create the header file 'src/date.h'"
)
add_dependencies(date date.exe) # Compile "src/date.c", if necessary.
```

> Please note that the recipe for target `date` is executed **unconditionally**
> (whenever CMAKE encounters a target that depends on it).
    
Add a target for an executable. 

```cmake
add_executable(the_executable.exe
        src/source1.c
        src/source2.c
        src/date.h)
```

> Please note that the executable depends on `src/date.h`. This dependency is important
> for two reasons:
> * you want the executable to be (re)compiled whenever the file `src/date.h` changes.
>   And keep in mind that `src/date.h` is regenerated **unconditionally** upon all CMAKE execution.
> * you must add the dependency `src/date.h` if you want CLion to know about this file. 

You want the file `src/date.h` to be **unconditionally** (re)generated (even if it already exists).

```cmake
add_dependencies(the_executable.exe date)
```

> `the_executable.exe` depends on `date`.
> `date` recipe is executed unconditionally (because `date` is not a file)
> => `src/date.h` is (re)generated unconditionally
> => `the_executable.exe` is (re)generated unconditionally

> More complete example: [here](version.md)

#### You want the source file to be created "when appropriate"

Use the instruction `ADD_CUSTOM_COMMAND`.

```cmake
# rm -f cmake_tester src/main.c && cmake . && make
cmake_minimum_required(VERSION 3.24)
project(cmake_tester C)

set(CMAKE_C_STANDARD 99)

# DEPENDS: whenever `code-gen.pl` is modified, "src/main.c" is regenerated.
# WORKING_DIRECTORY: useful option. Specify the working directory.
#
# WARNING: you don't need to add "bash -c" before the commands to execute.
#          ex: bash -c "echo \"Generate the file main.c\""
#          If you do that, you may face strange behaviors.

ADD_CUSTOM_COMMAND(
        OUTPUT src/main.c
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS code-gen.pl
        COMMAND echo "Generate the file main.c"
        COMMAND perl code-gen.pl > src/main.c)

# If the file "src/main.c" does not exist, then it will be generated.
ADD_EXECUTABLE(cmake_tester src/main.c)
```

### Dealing with CLION builds organisation

If you build a DEBUG release, CLION will work in a subdirectory. For example:

```bash
$ tree tmp.azlAwkqXtM
tmp.azlAwkqXtM
|-- CMakeLists.txt
|-- Makefile
|-- bin
|-- cmake-build-debug
|-- cmake-build-debug-ubuntu-jammy-dev
|   |-- CMakeCache.txt
|   |-- Makefile
|   |-- bin
|   `-- lib
`-- tools
    |-- concat-configure.pl
    `-- concat.pl
```

The Makefile that will processed is the one located within the directory 
`cmake-build-debug-ubuntu-jammy-dev`. And, as you can see, the directory `tools` (not `src`) is not present in this directory. This may cause problems. In order to work around this weird behavior, you can use a path prefix (before all paths).

```cmake
if (EXISTS ${CMAKE_BINARY_DIR}/tools)
    set(PREFIX_PATH .)
else()
    set(PREFIX_PATH ..)
endif()
message( NOTICE "Prefix path is \"${PREFIX_PATH}\" (used for *CLION* builds only)"  )
```

# ANNEXE

### Get the compiler list of search paths for headers and libraries

```bash
echo | gcc -E -Wp,-v - | grep -v "# "
```
* -E: Stop after the preprocessing stage; do not run the compiler proper. The output is in the form of preprocessed source code, which is sent to the standard output. Input files which don't require preprocessing are ignored.
* -Wp: You can use `-Wp,option` to bypass the compiler driver and pass option directly through to the preprocessor. 

### CLION troubleshooting while using the remote mode

#### Header file not found

You may experience the following problem while using CLION in remote mode.

* CMAKE is well configured.
* The project compiles.
* But within the CLION editor, a header file cannot be found.

The reason for this error is that CLION did not upload the missing header file from the Docker container. You need to synchronise the local copy of the build environment with the one on the container.

![](doc/clion-header-not-found.png)

> In the screenshot above, CLION cannot find the header file "`curl.h`".

In this case, you probably need to "resync with remote hosts".

`Tools` => `Resync with remote hosts`

See this document: [Resync header search paths](https://www.jetbrains.com/help/clion/remote-projects-support.html#resync)

#### You changed the CMAKE specification file (CMakeLists.txt), but "nothing happens"

You need to reload the CMake project: `Tools` => `CMake` => `Reload CMake Project`

#### You did everything described above, but the problem persists

You may need to reset the cache and reload the CMake project: `Tools` => `CMake` => `Reset Cache and Reload Project`

### Good links

* [CMake FAQ](https://gitlab.kitware.com/cmake/community/-/wikis/FAQ)
