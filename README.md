# Software-Engineering-Project

Organization:
https://trello.com/b/1gmhWOCT/image-segmentation

Online report:
https://www.overleaf.com/3626672ysbgxf

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
