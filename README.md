# usvg-scenes #

This folder contains the test scenes of the paper:

> *Xingze Tian, Tobias Günther*  
**Unified Smooth Vector Graphics: Modeling Gradient Meshes and Curve-based Approaches Jointly as Poisson Problem**  

The demo code shows how to read the XML scenes in C++.
The implementation was tested on MSVC 19, GCC 11-13, and Clang 14. 
A CMake file is provided to compile the program.
Note that the paths to the XML files might have to be adjusted, depending on the working directory of the platform.

Files:
- `CMakeLists.txt` *Contains the CMake script for cross-platform compilation.*
- `main.cpp` *Contains the entry function of the program.*
- `reader.hpp` *Class that reads a scene from an XML file.*
- `scenes/` *Contains the XML files.*