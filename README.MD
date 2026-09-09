## General design choices
I wanted to do a cpu renderer since I felt like making a graphics pipeline from scratch to help strengthen my understanding of the math involved in rendering. Some of the specific implementation choices were made as follows: I decided to use Neon intrinsics as despite owning an ARM laptop for a while now I've never had a good excuse to write code using them. This just felt like a perfect use case. I opted to just do a normal z buffer rasteriser impementation since I wanted to focus mostly on the math and therefore didn't want to get too fancy with my implementation. This project should be viewed through the lens that everything is intended as just a learning experience.

There were some things that I would have done differently with hindsight. I likely would have stored more of my data in SoA to avoid some of the interleaving and deinterleaving I had to do. Even though Neon makes the process very simple with built-in instructions, as far as I can tell there is still a performance overhead. With hindsight it may also have been a good idea to have the math library just mutate the original matricies rather than creating new ones, as much as I wanted to write everything there as pure functions.

# Extension Ideas
I decided to do this project with a very specific deadline and therefore a lot of ideas were left on the cutting room floor. Here's a couple of them that might make it into a future 2.0 project if I decide to come back to this Repo.

## runtime editing features
Adding some GUI options to edit transformations and camera positions during runtime would be a good extra feature.

## camera rotation
I did some tests with this and I actually have a source for the math. I just decided to move on from the vertex shader to make sure I had time to get rest done (which turned out to be the right choice). Here's the references I found: 

https://blog.mkari.de/posts/cam-transform/ 
https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function/framing-lookat-function.html

Adding this feature wouldn't take too long and if I ever come back to iterate on this repo will likely be the first addition.

## Lighting
Adding some sort of basic lighting model, even something as simple as a single light direction dot product with vertex normal = intesity. Just any lighting solution. The renderer was already pretty much brushing up on 30 fps on my computer with the main test models I was using, so adding more expensive per pixel calculations seemed like a bad idea. You could just try to calculate it per vertex and then interpolate to save on calculations.
## Refactoring the math
In my opinion this is one of those things that would be high priority if this was supposed to be a real product. maintaining a codebase with 5 seperate near-identical matrix multiplications sounds like a pain and could easily be fixed with a few helper functions. But since this is a learning project I decided to write out all my Neon code any time I used it so that I could practice.

## Potential optimisation
There are some ideas for how to optimise things. The most obvious idea is multithreading. I didn't do multithreading because of time constraints (and to be honest I was curious how well it would run).

Another good idea is to change the interpolation math. For example during interpolation we do the full 3 multiply accumulates and a multiplication for each pixel. We could instead precompute interpolation gradients for each vertex attribute and then do the same DDA esque method we use to calculate our edge fuction outputs to instead do 2 multiply accumulates and an addition per pixel. We could also recalculate the base value to interpolate from in this method for each quad of 4 pixels instead of once at the start of the traingle if we want to multithread this. I've seen a mention of the idea of computing screen linear plane equations for each of the interpolated attributes in this NVidia [paper](https://research.nvidia.com/sites/default/files/pubs/2011-08_High-Performance-Software-Rasterization/laine2011hpg_paper.pdf), which would be a good way to compute the required derivatives and base values needed for this method, instead of trying to compute partial derivates from the barycentric interpolation. I didn't do the interpolation this way since I wanted the barycentric math to be clearly on display for anyone reading the code, so it matches the math you see in the references. I usually reread my own projects with the references I used to review ideas so this is more for me than anyone else.

As mentioned earlier avoiding some of the data interleaving is likely to also add some performance as well. however it is worth noting that only a single ld4 instruction is used in the math pipeline so the savings would mostly come from further refactoring such as rewriting the draw tri function so it can setup 4 triangles at once since it would be easier to vectorise with that data layout.
# External Library use
## stb_image.h
When it came time to read in a texture file for the model, I needed a solution for reading in a Png file. I could have wrote a png reader but that doesn't fall under the scope of this project, and would be unnecesary extra effort when an external library for the task already exists.

## SDL
I just needed a library that opened a window and drew a list of pixels colours to it, SDL fit my needs perfectly.

I mostly figured out the base SDL stuff from this rasterizer tutorial that I didn't end up following past the initial setup: https://lisyarus.github.io/blog/posts/implementing-a-tiny-cpu-rasterizer-part-2.html

# references
# Sources that taught me practically everything I needed for this projects

## The sites I owe pretty much everything to
https://www.scratchapixel.com/index.html
https://fgiesen.wordpress.com/

## Vertex transformations
https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrix-introduction.html
https://www.scratchapixel.com/lessons/3d-basic-rendering/computing-pixel-coordinates-of-3d-point/perspective-projection.html
## Barycentric and interpolation math
https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
https://arxiv.org/pdf/1308.1279
https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation//visibility-problem-depth-buffer-depth-interpolation.html
## perspective correct texture interpolation
https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation//perspective-correct-interpolation-vertex-attributes.html

A couple of research papers I read about the topic to see some more examples of the math.
https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8891846
https://www.researchgate.net/publication/2798677_Interpolation_for_Polygon_Texture_Mapping_and_Shading

I really recommend this next one by Chris Hecker as it goes through the math in a way that really informed my own intuition on the topic. His first article goes through the full explanation of why we interpolate with the reciprocal of the depth in a very straightforward manner which made understanding the ideas much easier.
https://chrishecker.com/images/4/41/Gdmtex1.pdf
https://chrishecker.com/images/9/97/Gdmtex2.pdf
## rasterization
https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/overview-rasterization-algorithm.html
https://research.nvidia.com/sites/default/files/pubs/2011-08_High-Performance-Software-Rasterization/laine2011hpg_paper.pdf
## Neon SIMD
https://support.arm.com/documentation/102467/0201
https://henryndubuaku.github.io/maths-cs-ai-compendium/chapter%2016%3A%20SIMD%20and%20GPU%20programming/02.%20ARM%20and%20NEON/#neon-128-bit-simd
https://github.com/thenifty/neon-guide/blob/master/README.md
# instruction reference
https://developer.arm.com/architectures/instruction-sets/intrinsics#q=


# things that were more implementation details than foundational knowledge

## aligned memory allocator implementation adapted from
https://stackoverflow.com/questions/60169819/modern-approach-to-making-stdvector-allocate-aligned-memory

## OBJ reader
https://www.scratchapixel.com/lessons/3d-basic-rendering/obj-file-format//obj-file-format.html