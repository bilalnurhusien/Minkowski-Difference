
## Build:
	
	$ make

## Usage:

	$ ./minkowski <num-of-polygon1-vertices> <num-of-polygon2-vertices>
	
	Use CSV input file to draw polygons:
	
	$ ./minkowski -f <input-file>
	
## Example:

	$./minkowski 6 5 # For a hexagon and a pentagon

	$./minkowski -f inputDataExample.txt

## Library Installation:

	For rendering images, please install SFML library.

	$ sudo apt-get install libsfml-dev

## Output: 

## For two pentagons
	$ ./minkowski 5 5 

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffPentagon.png)

## For a hexagon and a square
	$ ./minkowski 6 4 

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffSquareHexagon.png)

        # For input file
        $ ./minkowski -f inputDataExample.txt

![alt text](https://github.com/bilalnurhusien/Minkowski-Difference/blob/master/images/MinkowskiDiffInputExample.png)
