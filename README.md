# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).


### Project 5 Design

#### Camera Data
My camera design was very similar to the camera design from the ray tracing project. I took the same class and added more fields including the projection matrix and the width angle. I also added more methods to update the camera when the screen was resized or when the near and far planes changed

#### Shape Implementations
For my shape implementations I took the shapes that we implemented in lab 8 and brought them over, then I added support for Cones and Cylinders. The major challenges here were deciding how to tesselate the bases and how to calculate the surface normals at the top of the Cone. To tesselate the flat spherical bases I divided it up into equally sized triangles based on m_param2, so however many slices would be equal to however many triangles there are on the bases. To calculate the surface normal at the top of the Cone I manually calculated the x and z direction, normalized, and chose 0.5 as the y direction based on the slope of the cone, then normalized again.

#### Data Handling
Since there are at most 4 different sets of vertices I chose to store my VBOs and VAOs in a map, which maps the primitive type to the VBO or VAO. I also created a class called VertexGenerator to recompute the vertices as needed when the tesselation parameters change. This minimizes the number of sets of vertices I need to create and keep around as well as minimizing the number of VBOs/VAOs I need to create.

#### Shaders
I have my vertex and my fragment shaders which are very similar to the shaders we implemented in lab 10. The vertex shader simply transforms the object space position and normal into world space and passes it onto the fragment shader. The fragment shader uses the normal along with uniforms passed in to calculate the final color of each fragment. I pass a lot of uniforms into the fragmenet shader in realtime.cpp and this is something I would've liked to avoid, but with comments about each uniform, I don't think the choice to do this was all that bad.

### Known Bugs
N/A

### Extra Credit
N/A
