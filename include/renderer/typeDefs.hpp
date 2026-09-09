#pragma once
#include <vector>
#include <array>
#include <limits>
#include <new>
#include <exception>

namespace renderer {

    //A custom allocator to ensure vectors are allocated to a 16 byte aligned memory address
    //Because Neon Q registers are 128 bits, a 16 byte alignment is the best way to
    //guarantee efficient SIMD
    //for source see readme for a link to the stack overflow page I learned about this from
    template<typename ElementType, std::size_t ALIGNMENT_IN_BYTES>
    class AlignedAllocator{
        private:
            static_assert(ALIGNMENT_IN_BYTES >= alignof(ElementType));
        
        public:
            using value_type = ElementType;
            static std::align_val_t constexpr ALIGNMENT { ALIGNMENT_IN_BYTES };
            template<class OtherElementType>
            struct rebind{
                using other = AlignedAllocator<OtherElementType, ALIGNMENT_IN_BYTES>;
            };
        
        public:
            constexpr AlignedAllocator() noexcept = default;

            constexpr AlignedAllocator( const AlignedAllocator& ) noexcept = default;

            template<typename U>
            constexpr AlignedAllocator( AlignedAllocator<U, ALIGNMENT_IN_BYTES> const& ) noexcept{}
            

            [[nodiscard]] ElementType* allocate( std::size_t nElementsToAllocate ) {
                if ( nElementsToAllocate > std::numeric_limits<std::size_t>::max() / sizeof( ElementType ) ) {
                    throw std::bad_array_new_length();
                }

                auto const nBytesToAllocate = nElementsToAllocate * sizeof( ElementType );
                return reinterpret_cast<ElementType*>(::operator new[]( nBytesToAllocate, ALIGNMENT ) );
            }

            void deallocate(ElementType* allocatedPointer,[[maybe_unused]] std::size_t  nBytesAllocated ){
                ::operator delete[]( allocatedPointer, ALIGNMENT );
            }
    };

    template<typename T, std::size_t ALIGNMENT_IN_BYTES = 16>
    using AlignedVector = std::vector<T, AlignedAllocator<T, ALIGNMENT_IN_BYTES> >;

    //a simple 4 float vector
    //I choose to do it this way instead of with an array so I could index by xyzw/rgba component
    //I added a way to index into it like a JS object with a char in the [] but I actually ended up not using anywhere though
    struct vec4 {
        float x;
        float y;
        float z;
        float w;

        float& operator[](char const component) {
            switch (component) {
                case 'x':
                case 'X':
                case 'r':
                case 'R':
                    return x;
                case 'y':
                case 'Y':
                case 'g':
                case 'G':
                    return y;
                case 'z':
                case 'Z':
                case 'b':
                case 'B':
                    return z;
                case 'w':
                case 'W':
                case 'a':
                case 'A':
                    return w;
                default:
                    throw std::out_of_range("Accesing non-existent vec4 component");
            }
        }
    };

    //a vector of 3 floats
    struct vec3 {
        float x;
        float y;
        float z;

        float& operator[](char const component) {
            switch (component) {
                case 'x':
                case 'X':
                case 'r':
                case 'R':
                    return x;
                case 'y':
                case 'Y':
                case 'g':
                case 'G':
                    return y;
                case 'z':
                case 'Z':
                case 'b':
                case 'B':
                    return z;
                default:
                    throw std::out_of_range("Accesing non-existent vec4 component");
            }
        }
    };

    //a simple 2 float vector
    struct vec2
    {
        float x;
        float y;

         float& operator[](char const component) {
            switch (component) {
                case 'x':
                case 'X':
                case 'u':
                case 'U':
                    return x;
                case 'y':
                case 'Y':
                case 'v':
                case 'V':
                    return y;
                default:
                    throw std::out_of_range("Accesing non-existent vec2 component");
            }
        }
    };

    //A face on the model
    //has a position and uv index for each vertex in the face
    struct Face {
        std::array<int, 3> Verts;
        std::array<int, 3> UVs;
    };

    using AlignedVec4 = AlignedVector<vec4, 16>;
    using AlignedVec3 = AlignedVector<vec3, 16>;
    using AlignedVec2 = AlignedVector<vec2, 16>;
    using AlignedFaces = AlignedVector<Face, 16>;

    //Struct for holding all of a models data in one place
    //Has the following fields
    //faceVertices - the faces of the model
    //vertexPositions - the object space positions of each vertex in the model
    //vertexUVs - the UV values of the vertexes in the model (Since this is generated from an obj file this is not guaranteed to be aligned with the positions)
    //transform - a 4x4 matrix that stores the models object to world transform
    //albedo a 1024x1024 png texture
    struct model {
        AlignedFaces faceVerticies;
        AlignedVec4 vertexPositions;
        AlignedVec2 vertexUVs;
        std::array<float, 16> transform;
        std::vector<uint32_t> albedo;
    };

    //A struct which contains all the objects in the scene
    //has the following fields
    //models - a list of the models in the scene
    //cameraPos - a position vector for the camera
    //fov - the camera field of view
    //near - the near clipping plane distance
    //far - the far clipping plane distance
    struct scene {
        std::vector<model> models;
        vec4 cameraPos;
        float fov, near, far;
        
    };
}