# Final Project: Marching Cubes

## Description
This project builds upon the realtime pipeline from projects 5 & 6. It adds a new shape called "MarchingCube" which captures the marching cube algorithm. Each MarchingCube contains specific parameters that can be changed, but the GUI from projects 5 & 6 have also been repurposed to modify the MarchingCube parameters. From the GUI, the user can modify the number of levels, which indicates how many cubes high the object should be. The size of base parameter determines how many smaller cubes make up the width and depth of the environment. The fixed base size checkbox determines if we should use a fixed base size. If this field is checked, then the box will always have a base size of 1 world space unit x 1 world space unit centered at 0, 0 and if not, then the base grows to (Size of Base * 0.1) x (Size of Base * 0.1). The noise checkbox will generate new values for the cube vertices everytime it is checked off, the rotate parameter causees the camera to rotate at a fixed speed, and the terrain parameter colors the cubes based on their height.


## Video
https://www.youtube.com/watch?v=sb18Sy7xzC8

## Sources
Edge Table: http://paulbourke.net/geometry/polygonise/
Triangle Table http://paulbourke.net/geometry/polygonise/
