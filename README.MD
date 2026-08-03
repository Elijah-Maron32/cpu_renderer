## General design choices
I wanted to do a cpu renderer since I felt like making a graphics pipeline from scratch to help strengthen my understanding of the math involved in rendering. Some of the specific implementation choices were made as follows: I decided to use Neon intrinsics as despite owning an ARM laptop for a while now I've never had a good excuse to write code using them. This just felt like a perfect use case. I opted to just do a normal z buffer rasteriser impementation since I wanted to focus mostly on the math and therefore didn't want to get too fancy with my implementation.

## WIP Data structres

As nice as it would be to create neat little struct representations of everything I plan to use NEON SIMD instructions so we'll see

# Pixel
for the sake of both textures and the pixel buffer a pixel in this rendered is defined as a single uint32_t with a single byte each for RGBA, Alpha is unlikely to be used at any point here but could be useful so I would prefer to leave it in. The pixel buffer and any textures will be defined by contiguos arrays of these pixels

# Model
A model will be taken as a struct containing an array of verticies, an array of faces, an array of UV co-ordinates and at least one texture (we'll see how many more as we go on)




## External Library use
# stb_image.h
When it came time to read in a texture file for the model, I needed a solution for reading in a Png file. I could have wrote a png reader but that doesn't fall under the scope of this project, and would be unnecesary extra effort when an external library for the task already exists.

## references
# aligned memory allocator implementation adapted from
https://stackoverflow.com/questions/60169819/modern-approach-to-making-stdvector-allocate-aligned-memory

# OBJ reader
https://www.scratchapixel.com/lessons/3d-basic-rendering/obj-file-format//obj-file-format.html