#pragma once
#include <vector>
#include <array>
#include <limits>
#include <new>
#include <exception>

namespace renderer {
    //32 bit colour bitmasks
    #define RED_BIT_MASK 0xF000
    #define GREEN_BIT_MASK 0x0F00
    #define BLUE_BIT_MASK 0x00F0
    #define ALPHA_BIT_MASK 0x000F

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

    template<typename T, std::size_t ALIGNMENT_IN_BYTES = 64>
    using AlignedVector = std::vector<T, AlignedAllocator<T, ALIGNMENT_IN_BYTES> >;

    //a simple 4 float vector
    //I choose to do it this way instead of with an array so I could index by xyzw/rgba component
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
    }; //___attribute__((alligned(128)));

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
    //I choose to do it this way instead of with an array so I could index by xy/uv component
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
    }; //___attribute__((alligned(128)));

    // struct matrix4x4
    // {
    //     float x[4];
    //     float y[4];
    //     float z[4];
    //     float w[4];
    // };
    
    

    struct vertex {
        vec4 const position;
        //UV
        //Normal?
    };

    struct Face {
        std::array<int, 3> Verts;
        std::array<int, 3> UVs;
    };

    using AlignedVec4 = AlignedVector<vec4, 16>;
    using AlignedVec3 = AlignedVector<vec3, 16>;
    using AlignedVec2 = AlignedVector<vec2, 16>;
    using AlignedFaces = AlignedVector<Face, 16>;

    static_assert(std::is_same_v<AlignedVec4::value_type, vec4>);

    struct model {
        AlignedVec4 vertexPositions;
        //AlignedVec4 vertexNormals;
        AlignedVec2 vertexUVs;
        AlignedFaces faceVerticies;
        std::array<float, 16> transform;
        //std::array<uint32_t, 4096> albedo;
    };

    struct scene {
        std::vector<model> models;
        vec4 cameraPos;
        float fov, near, far;
        
    };

    //potential helper functions worth having
    //vectorToPixel()
    //pixelToVector()
    //normaliseVector()
    //transformVector()
}