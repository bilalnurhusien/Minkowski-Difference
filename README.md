
### Build:
	
	$ make

### Usage:

	$ ./minkowski <num-of-robot-vertices> <num-of-obstacle-vertices>
	
	Use CSV input file to draw polygons:
	
	$ ./minkowski -f <input-file>
	
### Library Installation:

	For rendering images, please install SFML library.

	$ sudo apt-get install libsfml-dev

### Examples: 

#### Square robot and a hexagon obstacle:
	$ ./minkowski 4 6

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffSquareHexagon.png)

#### Pentagon robot and pentagon obstacle:
	$ ./minkowski 5 5 

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffPentagon.png)

#### Input file
        $ ./minkowski -f inputDataExample.txt

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffInputExample.png)
