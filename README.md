# Final Project: Marching Cubes

## Description
This project builds upon the realtime pipeline from projects 5 & 6. It adds a new shape called "MarchingCube" which captures the marching cube algorithm. In the marching cube algorithm, there exists a 3D grid of uniformly spaced points and each point can be thought of as being part of the cubes that surround it. Each point receives a value, in this case the value is determined by Perlin noise, and given this value and a user defined threshold, we can determine if the point is encased in a shape. For each cube in our grid we can find which vertices are inside/outside of a shape and draw our own vertices along the edges that the shape intersects. Normally, the point values would be used in interpolation to determine the exact location of intersection, but in my implementation I chose to weigh each point equally, leading to blockier shapes. Additioanlly, there's considerable slowdown once the size of the base reaches around 30 and that's something I hoped to fix, but couldn't manage in time.

## GUI Parameters
- Number of Layers: The height of the environment in cubes
- Size of Base: Both the width and depth of the environment in cubes
- Fixed Base Size: Fixes the base into a 1 x 1 plane centered at 0,0
- Noise: Regenerates the vertex values using Perlin noise
- Rotate: Rotates the camera at a fixed speed
- Terrain: Colors the environment based on height

Threshold value starts at 0.0.

Press Q to lower the threshold value and press E to increase the threshold value.

## Video
https://www.youtube.com/watch?v=sb18Sy7xzC8

## Sources
Primary Source: https://people.eecs.berkeley.edu/~sequin/CS285/PAPERS/Lorensen_Cline.pdf

Edge Table: http://paulbourke.net/geometry/polygonise/

Triangle Table http://paulbourke.net/geometry/polygonise/
