namespace renderer {
    //32 bit colour bitmasks
    #define RED_BIT_MASK 0xF000
    #define GREEN_BIT_MASK 0x0F00
    #define BLUE_BIT_MASK 0x00F0
    #define ALPHA_BIT_MASK 0x000F

    struct vec4 {
        float x;
        float y;
        float z;
        float w;
    } ___attribute__((alligned(64)));

    struct vertex {
        vec4 const position;
        //UV
        //Normal?
    };

    struct model {
        vertex *const verticies;
        //faces
        //textures
    };

    //potential helper functions worth having
    //vectorToPixel()
    //pixelToVector()
    //normaliseVector()
}