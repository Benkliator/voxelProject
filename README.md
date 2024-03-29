# voxelProject

To run this program you will need GLFW, C++17, as well as OpenGL 3.3

For further development be wary:

TODO:
Use some secondary buffer for visibility to avoid rendering unseen chunks. Read up on glBufferSubData and maybe store an index for each chunk's data in the vertex and index arrays.

Make lighting actually work.

Store blocks in an actual structure to make visibility checks faster.

Create chunk mesh in a smarter way.

CHUNK UPDATE NOTES:

Update chunks whenever player moves from one chunk to another using glBufferSubData

Store an index into the batch buffer used in the rendering class for each loaded chunk.
