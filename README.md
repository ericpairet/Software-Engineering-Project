# Software-Engineering-Project

Organization:
https://trello.com/b/1gmhWOCT/image-segmentation

> This software is free: you can redistribute it and/or modify
> it under the terms of the GNU General Public License Version 3,
> as published by the Free Software Foundation.
> 
> This software is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
> 
> You should have received a copy of the GNU General Public License
> Version 3 in the file COPYING that came with this distribution.
> If not, see &lt;<http://www.gnu.org/licenses/>&gt;.

===================================================

Laplacian Seed Based Image Segmentation
----------------------------------------------
This software is cross-platform, developed on Linux, Mac OSx, and Windows

Online Documentation
--------------------

To find more in-depth and up-to-date information about this project
(including installation and configuration), please visit the [Documentation Page](http://sepehr.im/sbis/)
or this [Documentation Page](http://voidminded.com/sbis/).
 

Software Requirements
---------------------

- cMake >= 2.8 or qMake
- g++, clang++ or vc++
- QT >= 5.0 
- Eigen2 or Eigen3 with unsuported packages
- openCV
- GTest (Google Test libraries)


Compilation
-----------

For ease of use this project could be beiu using qMake or cMake.

Using qMake : 

You can use the "CIS.pro" file as the project file for Qt. project 
you have to update the "include" folders and "library" folders in
"CIS.Pro" file based on your own system for following libraries :

- Eigen
- openCV
- GTest

Using cMake :

Move to project root folder folder:
``cd CIS``

Create a build folder:
``mkdir build``

Move to the created folder:
``cd build``

Create the `MakeFile` via cmake:
``cmake ..``

If the cMake can not find the requirements you have to give them manually 
either by usinf `cmake-gui` or `-DCMAKE_MODULE_PATH` flag

Required packages in case of not being found automatically:
* Select Qt5Widgets.cmake as Qt.5 requirement
* From openCV you need `libopencv_core` and `libopencv_imgproc` libraries
* From Google Test `libgtest` and `libgtest_main`
* Give Eigen main folder as address of eigen requirement

Note : Eigen is a bit tricky with cMake as it is a completely header only,
if the cMake could not find it automatically use `-DCMAKE_MODULE_PATH=address/to/eigen/folder`
flag to give the address to cmake.

On linux, Mac, or Windows build the code by running:
``make``

The project *should* build without errors or warnings.

On Windows it's possible to use:
``make_vcproj``

that script will call cmake-gui.

Also on Mac and Windows one can also use cmake-gui directly, choosing
the project root as the source directory, and build folder as
build directory.


Running
-------

* cMake creates a `bin` folder in main project folder `CIS`
from there you can run the program by :

``./seedBasedSegmentation``

GTesting
--------

* For gTesting you need to enter the ``developer mode`` of the program by pressing `Ctrl+D`
this will show the overall GTest result in program's debugger and the detailed process of 
all the unit tests in terminal which has run the progeam.


