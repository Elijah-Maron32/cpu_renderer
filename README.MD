## WIP Data structres

As nice as it would be to create neat little struct representations of everything I plan to use NEON SIMD instructions so we'll see

# Pixel
for the sake of both textures and the pixel buffer a pixel in this rendered is defined as a single uint32_t with a single byte each for RGBA, Alpha is unlikely to be used at any point here but could be useful so I would prefer to leave it in. The pixel buffer and any textures will be defined by contiguos arrays of these pixels

# Model
A model will be taken as a struct containing an array of verticies, an array of faces, an array of UV co-ordinates and at least one texture (we'll see how many more as we go on)






## references
# aligned memory allocator implementation adapted from
https://stackoverflow.com/questions/60169819/modern-approach-to-making-stdvector-allocate-aligned-memory
