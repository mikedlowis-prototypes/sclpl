SCLPL
==============================================

About This Project
----------------------------------------------

License
----------------------------------------------
Unless explicitly stated otherwise, all code and documentation contained within
this repository is released under the BSD 2-Clause license. The text for this
license can be found in the LICENSE.md file.

Build Instructions
----------------------------------------------

## Build and Test the Compiler

Execute the following command to build the compiler executable and run all tests
on it:

    make all

## Build the Compiler and Skip the Tests

The test suite for the compiler uses Ruby and Rspec. It is conceivable that an
end user may not have these dependencies installed and may therefore wish to
build the compiler without fully testing it. This may be accomplished by
running the following command:

    make sclpl
