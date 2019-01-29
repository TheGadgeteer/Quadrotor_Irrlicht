How to compile:

- download the Irrlicht engine source code from http://irrlicht.sourceforge.net/
- open the project in Visual Studio  (created in Visual Studio Community 2017)
- add the Irrlicht include path "irrlicht-<version>/include" under project properties->vc++ directories->include
- add the Irrlicht library path "irrlicht-<version>/lib" under project properties->vc++ directories->libraries
- if it says "Windows SDK" not available during compiling, retarget the project to an available Windows SDK.

To execute the program, it needs to find the irrlicht.dll file which can be found in "irrlicht-<version>/bin".
The easiest way is to just copy-past the dll in the same folder as the .exe file.
