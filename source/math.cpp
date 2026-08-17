#include <renderer/math.hpp>
#include <arm_neon.h>
#include <cmath>
#include <iostream>
#include <numbers>

namespace renderer {

std::array<float, 16> applyTranslation(std::array<float, 16> const& transform, renderer::vec4 const& translation) {
    std::array<float, 16> resultsMatrix;
    std::copy(transform.begin(), transform.end(), resultsMatrix.begin());
    
    resultsMatrix[12] += translation.x;
    resultsMatrix[13] += translation.y;
    resultsMatrix[14] += translation.z;

    return resultsMatrix;
}

std::array<float, 16> applyRotationX(std::array<float, 16> const& transform, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    std::array<float,16> rotation = {
        1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1
    };

    //multiply rotation matrix in
    std::array<float, 16> resultMatrix;

    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    float32x4_t rotationC0;
    float32x4_t rotationC1;
    float32x4_t rotationC2;
    float32x4_t rotationC3;

    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    rotationC0 = vld1q_f32(rotation.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, rotationC0, 0);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, rotationC0, 1);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, rotationC0, 2);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, rotationC0, 3);
    vst1q_f32(resultMatrix.data(), resultC0);

    rotationC1 = vld1q_f32(rotation.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, rotationC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, rotationC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, rotationC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, rotationC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    rotationC2 = vld1q_f32(rotation.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, rotationC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, rotationC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, rotationC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, rotationC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    rotationC3 = vld1q_f32(rotation.data()+12);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC0, rotationC3, 0);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC1, rotationC3, 1);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC2, rotationC3, 2);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC3, rotationC3, 3);
    vst1q_f32(resultMatrix.data()+12, resultC3);

    return resultMatrix;
}

std::array<float, 16> applyRotationY(std::array<float, 16> const& transform, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    std::array<float,16> rotation = {
        c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1
    };

    //multiply rotation matrix in
    std::array<float, 16> resultMatrix;

    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    float32x4_t rotationC0;
    float32x4_t rotationC1;
    float32x4_t rotationC2;
    float32x4_t rotationC3;

    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    rotationC0 = vld1q_f32(rotation.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, rotationC0, 0);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, rotationC0, 1);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, rotationC0, 2);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, rotationC0, 3);
    vst1q_f32(resultMatrix.data(), resultC0);

    rotationC1 = vld1q_f32(rotation.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, rotationC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, rotationC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, rotationC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, rotationC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    rotationC2 = vld1q_f32(rotation.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, rotationC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, rotationC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, rotationC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, rotationC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    rotationC3 = vld1q_f32(rotation.data()+12);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC0, rotationC3, 0);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC1, rotationC3, 1);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC2, rotationC3, 2);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC3, rotationC3, 3);
    vst1q_f32(resultMatrix.data()+12, resultC3);

    return resultMatrix;
}

std::array<float, 16> applyRotationZ(std::array<float, 16> const& transform, float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    
    std::array<float,16> rotation = {
        c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    //multiply rotation matrix in
    std::array<float, 16> resultMatrix;

    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    float32x4_t rotationC0;
    float32x4_t rotationC1;
    float32x4_t rotationC2;
    float32x4_t rotationC3;

    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    rotationC0 = vld1q_f32(rotation.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, rotationC0, 0);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, rotationC0, 1);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, rotationC0, 2);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, rotationC0, 3);
    vst1q_f32(resultMatrix.data(), resultC0);

    rotationC1 = vld1q_f32(rotation.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, rotationC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, rotationC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, rotationC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, rotationC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    rotationC2 = vld1q_f32(rotation.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, rotationC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, rotationC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, rotationC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, rotationC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    rotationC3 = vld1q_f32(rotation.data()+12);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC0, rotationC3, 0);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC1, rotationC3, 1);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC2, rotationC3, 2);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC3, rotationC3, 3);
    vst1q_f32(resultMatrix.data()+12, resultC3);

    return resultMatrix;
}

//given an existing transform matrix, a normailised direction vector and a normalised axis vector rotate towards the diretion on the given axis
std::array<float, 16> applyRotation(std::array<float, 16> const& transform, renderer::vec4 const& direction, renderer::vec4 const& axis) {
    float32x4_t d = vld1q_f32(&direction.x);
    float32x4_t a = vld1q_f32(&axis.x);
    float32x4_t dot = vmovq_n_f32(0);

    dot = vmulq_f32(d, a);
    float c = vaddvq_f32(dot);
    
    //cross product   
    //bit masks for swizzling 
    static const uint8_t mask1_data[16] = {4,5,6,7, 8,9,10,11, 0,1,2,3, 12,13,14,15};    // [Y, Z, X, W]
    static const uint8_t mask2_data[16] = {8,9,10,11, 0,1,2,3, 4,5,6,7, 12,13,14,15};    // [Z, X, Y, W]

    uint8x16_t mask1 = vld1q_u8(mask1_data);
    uint8x16_t mask2 = vld1q_u8(mask2_data);

    uint8x16_t d_bytes = vreinterpretq_u8_f32(d);
    uint8x16_t a_bytes = vreinterpretq_u8_f32(a);

    float32x4_t d_zxy = vreinterpretq_f32_u8(vqtbl1q_u8(d_bytes, mask2));
    float32x4_t a_yzx = vreinterpretq_f32_u8(vqtbl1q_u8(a_bytes, mask1));

    float32x4_t b_yzx = vreinterpretq_f32_u8(vqtbl1q_u8(d_bytes, mask1));
    float32x4_t a_zxy = vreinterpretq_f32_u8(vqtbl1q_u8(a_bytes, mask2));

    //compute cross product
    float32x4_t prod = vmulq_f32(a_yzx, d_zxy);//[a.y*b.z, a.z*b.x, a.x*b.y, a.w*b.w]
    prod = vmlsq_f32(prod, a_zxy, b_yzx);//[a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - , a.w*b.w - a.w*b.w]

    vec4 v;// = cross( z, d );
    vst1q_f32(&v.x, prod);
    
    const float k = 1.0f/(1.0f+c);

    std::array<float,16> rotation = 
    { v.x*v.x*k + c,     v.y*v.x*k - v.z,    v.z*v.x*k + v.y, 0,
    v.x*v.y*k + v.z,   v.y*v.y*k + c,      v.z*v.y*k - v.x, 0,
    v.x*v.z*k - v.y,   v.y*v.z*k + v.x,    v.z*v.z*k + c, 0,
    0, 0, 0, 1 };

    //multiply rotation matrix in
    std::array<float, 16> resultMatrix;

    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    float32x4_t rotationC0;
    float32x4_t rotationC1;
    float32x4_t rotationC2;
    float32x4_t rotationC3;

    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    rotationC0 = vld1q_f32(rotation.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, rotationC0, 0);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, rotationC0, 1);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, rotationC0, 2);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, rotationC0, 3);
    vst1q_f32(resultMatrix.data(), resultC0);

    rotationC1 = vld1q_f32(rotation.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, rotationC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, rotationC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, rotationC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, rotationC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    rotationC2 = vld1q_f32(rotation.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, rotationC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, rotationC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, rotationC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, rotationC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    rotationC3 = vld1q_f32(rotation.data()+12);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC0, rotationC3, 0);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC1, rotationC3, 1);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC2, rotationC3, 2);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC3, rotationC3, 3);
    vst1q_f32(resultMatrix.data()+12, resultC3);

    return resultMatrix;
};

//given an existing transform matrix and scaling factor apply the scaling uniformly
std::array<float, 16>  applyScaling(std::array<float, 16> const& transform, float const &scalar) {
    return applyScaling(transform, scalar, scalar, scalar);
};

//given an existing transform matrix and scaling factor per component apply the scaling
std::array<float, 16>  applyScaling(std::array<float, 16> const& transform, float const &scalarX, float const &scalarY, float const &scalarZ) {
    std::array<float, 16> resultMatrix;
    
    std::array<float, 16> scaleMatrix;
    scaleMatrix[0] = scalarX;
    scaleMatrix[5] = scalarY;
    scaleMatrix[10] = scalarZ;
    scaleMatrix[15] = 1.0;

    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    float32x4_t scaleC0;
    float32x4_t scaleC1;
    float32x4_t scaleC2;
    float32x4_t scaleC3;

    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    scaleC0 = vld1q_f32(scaleMatrix.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, scaleC0, 0);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, scaleC0, 1);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, scaleC0, 2);
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, scaleC0, 3);
    vst1q_f32(resultMatrix.data(), resultC0);

    scaleC1 = vld1q_f32(scaleMatrix.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, scaleC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, scaleC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, scaleC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, scaleC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    scaleC2 = vld1q_f32(scaleMatrix.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, scaleC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, scaleC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, scaleC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, scaleC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    scaleC3 = vld1q_f32(scaleMatrix.data()+12);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC0, scaleC3, 0);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC1, scaleC3, 1);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC2, scaleC3, 2);
    resultC3 = vfmaq_laneq_f32(resultC3, transformC3, scaleC3, 3);
    vst1q_f32(resultMatrix.data()+12, resultC3);

    return resultMatrix;
};

renderer::vec4 normalisedVector(renderer::vec4 const& v) {
    renderer::vec4 result;
    float32x4_t vec = vld1q_f32(&v.x);
    float32x4_t temp = vmovq_n_f32(0);

    temp = vmulq_f32(vec, vec);
    float sum = vaddvq_f32(temp);
    sum = std::sqrt(sum);

    temp = vmovq_n_f32(sum);
    temp = vdivq_f32(vec, temp);
    vst1q_f32(&result.x, temp);
    return result;
};

renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform, renderer::AlignedVec4 const& verticies){
    renderer::AlignedVec4 worldVerticies(verticies.size(), {0, 0, 0, 0});
    //worldVerticies.reserve(verticies.size());
    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    
    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12);
    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;
    
    size_t i = 0;
    size_t count = verticies.size();
    for (;i < (count & ~3); i += 4) {
        
        float32x4x4_t v = vld1q_f32_x4(reinterpret_cast<float const*>(verticies.data()) + (i*4));
        
        resultC0 = vmovq_n_f32(0);
        resultC1 = vmovq_n_f32(0);
        resultC2 = vmovq_n_f32(0);
        resultC3 = vmovq_n_f32(0);
        //this is just doing the vector transforms as if they're a 4x4 matrix so we can do 4 at once
        resultC0 = vfmaq_laneq_f32(resultC0, transformC0, v.val[0], 0);
        resultC0 = vfmaq_laneq_f32(resultC0, transformC1, v.val[0], 1);
        resultC0 = vfmaq_laneq_f32(resultC0, transformC2, v.val[0], 2);
        //for our co-ordinate vectors w is always one so no need to multiply
        resultC0 = vaddq_f32(resultC0, transformC3);
        
        resultC1 = vfmaq_laneq_f32(resultC1, transformC0, v.val[1], 0);
        resultC1 = vfmaq_laneq_f32(resultC1, transformC1, v.val[1], 1);
        resultC1 = vfmaq_laneq_f32(resultC1, transformC2, v.val[1], 2);
        resultC1 = vaddq_f32(resultC1, transformC3);
        
        resultC2 = vfmaq_laneq_f32(resultC2, transformC0, v.val[2], 0);
        resultC2 = vfmaq_laneq_f32(resultC2, transformC1, v.val[2], 1);
        resultC2 = vfmaq_laneq_f32(resultC2, transformC2, v.val[2], 2);
        resultC2 = vaddq_f32(resultC2, transformC3);
        
        resultC3 = vfmaq_laneq_f32(resultC3, transformC0, v.val[3], 0);
        resultC3 = vfmaq_laneq_f32(resultC3, transformC1, v.val[3], 1);
        resultC3 = vfmaq_laneq_f32(resultC3, transformC2, v.val[3], 2);
        resultC3 = vaddq_f32(resultC3, transformC3);
        
        float32x4x4_t v_result = {resultC0, resultC1, resultC2, resultC3};
        vst1q_f32_x4(reinterpret_cast<float *>(worldVerticies.data()) + (i*4), v_result);
    }
    
    //cleanup any remaining vectors that didn't fit in the previous operations
    for(;i < count; ++i) {
        float32x4_t v = vld1q_f32(reinterpret_cast<float const*>(verticies.data()) + (i*4));
        float32x4_t result = vmovq_n_f32(0);
        result = vfmaq_laneq_f32(result, transformC0, v, 0);
        result = vfmaq_laneq_f32(result, transformC1, v, 1);
        result = vfmaq_laneq_f32(result, transformC2, v, 2);
        result = vaddq_f32(result, transformC3);
        
        vst1q_f32(reinterpret_cast<float *>(worldVerticies.data()) + (i*4), result);
    }
    return worldVerticies;
};

// std::array<float, 16> getCameraTransform(renderer::vec4 const& cameraPos, float zAng)

renderer::AlignedVec4 divideW(renderer::AlignedVec4 const& cartesianVerticies) {
    renderer::AlignedVec4 homogenousVerticies(cartesianVerticies.size(), {0, 0, 0, 0});
    
    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;
    
    size_t i = 0;
    size_t count = cartesianVerticies.size();
    for (;i < (count & ~3); i += 4) {
        
        float32x4x4_t v = vld1q_f32_x4(reinterpret_cast<float const*>(cartesianVerticies.data()) + (i*4));
        
        resultC0 = vmovq_n_f32(v.val[0][3]);
        resultC1 = vmovq_n_f32(v.val[1][3]);
        resultC2 = vmovq_n_f32(v.val[2][3]);
        resultC3 = vmovq_n_f32(v.val[3][3]);

        resultC0 = vsetq_lane_f32(1, resultC0, 3);
        resultC1 = vsetq_lane_f32(1, resultC1, 3);
        resultC2 = vsetq_lane_f32(1, resultC2, 3);
        resultC3 = vsetq_lane_f32(1, resultC3, 3);        

        resultC0 = vdivq_f32(v.val[0], resultC0);
        resultC1 = vdivq_f32(v.val[1], resultC1);
        resultC2 = vdivq_f32(v.val[2], resultC2);
        resultC3 = vdivq_f32(v.val[3], resultC3);
        
        float32x4x4_t v_result = {resultC0, resultC1, resultC2, resultC3};
        vst1q_f32_x4(reinterpret_cast<float *>(homogenousVerticies.data()) + (i*4), v_result);
    }

    for(;i < count; ++i) {
        float32x4_t v = vld1q_f32(reinterpret_cast<float const*>(cartesianVerticies.data()) + (i*4));
        float32x4_t result = vmovq_n_f32(cartesianVerticies[i].w);
        result = vsetq_lane_f32(1, result, 3);
        result = vdivq_f32(v, result);
        
        vst1q_f32(reinterpret_cast<float *>(homogenousVerticies.data()) + (i*4), result);
    }
    return homogenousVerticies;
}

renderer::AlignedVec4 getProjectedCoordinates(AlignedVec4 const& verticies, vec4 const& cameraPos, float const fov, float const near, float const far, float const aspectRatio) {
    vec4 cameraOffset = {cameraPos.x * -1,cameraPos.y * -1, cameraPos.z * -1, 1.0};
    
    std::array<float, 16> cameraTransform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    cameraTransform = applyTranslation(cameraTransform, cameraOffset);
    // cameraTransform = applyRotationX(cameraTransform, -(std::numbers::pi/3));
    AlignedVec4 cameraSpaceVerticies = localToTransform(cameraTransform, verticies);

    const float top = tan(((fov/2)*(std::numbers::pi /180)))*near;
    const float bottom = -top;
    const float right = top * aspectRatio;
    const float left = -right;

    std::array<float, 16> perspectiveTransform = {
        2*near/(right-left), 0, 0, 0,
        0, 2*near/(top-bottom), 0, 0,
        0, 0, -((far + near)/(far - near)), -1,
        0, 0, -(2*far*near/(far - near)), 0
    };

    AlignedVec4 projectedCoordinates = localToTransform(perspectiveTransform, cameraSpaceVerticies);
    return divideW(projectedCoordinates);


};

renderer::AlignedVec4 getRasterCoords(AlignedVec4 const& verticies, int height, int width) {
    size_t count = verticies.size();
    AlignedVec4 rasterVerts(count, {0, 0, 0, 0});
   

    float32x4_t resultC0;
    float32x4_t resultC1;

    float32x4_t tempC0;
    float32x4_t tempC1;
    float32x4_t tempC2;
    float32x4_t tempC3;

    tempC0 = vmovq_n_f32(1);
    tempC1 = vmovq_n_f32(2);
    tempC2 = vmovq_n_f32(width);
    tempC3 = vmovq_n_f32(height);

    size_t i = 0;
    for(;i < (count & ~3); i += 4) {
        float32x4x4_t v = vld4q_f32(reinterpret_cast<float const*>(verticies.data()) + (i*4));
        

        resultC0 = vaddq_f32(v.val[0], tempC0);
        resultC0 = vdivq_f32(resultC0, tempC1);
        resultC0 = vmulq_f32(resultC0, tempC2);

        resultC1 = vsubq_f32(tempC0, v.val[1]);
        resultC1 = vdivq_f32(resultC1, tempC1);
        resultC1 = vmulq_f32(resultC1, tempC3);

        float32x4x4_t v_r = {resultC0, resultC1, v.val[2], v.val[3]};
        vst4q_f32(reinterpret_cast<float *>(rasterVerts.data()) + (i*4), v_r);
    }

    //for this one since we'd essentially be doing every element individually there isn't really a reason to do this cleanup with simd
    for(;i<count;++i){
        rasterVerts[i] = {(verticies[i].x + 1)/2 * width, (1 - verticies[i].y)/2 * height, verticies[i].z, verticies[i].w};
    }

    return rasterVerts;
};

//(c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x)
//a and b are verticies that we can keep constant (I.E. contigous load the values into a simd q reg once and leave it there)
//for c we're just computing points on the screen



}