SCLPL
==============================================

    Version:    0.1
    Created By: Michael D. Lowis
    Email:      mike@mdlowis.com

About This Project
----------------------------------------------

License
----------------------------------------------
Unless explicitly stated otherwise, all code and documentation contained within
this repository is released under the BSD 2-Clause license. The text for this
license can be found in the LICENSE.md file.

Requirements For Building
----------------------------------------------
The only external dependencies currently required to build this library are as
follows:

* Chciken Scheme
* SConstruct

Build Instructions
----------------------------------------------
This project uses SConstruct to build all binaries and libraries. To build the
software simply execute the following command at the root of the project:

    scons

Project Files and Directories
----------------------------------------------

    build/         This is the directory where all output files will be placed.
    source/        The source for the project.
    tests/         Unit test and mock files.
    tools/         Tools required by the build system.
    Doxyfile       Doxygen documentation generator configuration.
    LICENSE.md     The software license notification.
    premake4.lua   A premake4 configuration file for generating build scripts.
    project.vim    A VIM script with project specific configurations.
    README.md      You're reading this file right now!

