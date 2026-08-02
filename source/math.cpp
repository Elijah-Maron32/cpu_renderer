#include <renderer/math.hpp>
#include <arm_neon.h>
#include <cmath>

namespace renderer {

std::array<int, 16> applyTranslation(std::array<float, 16> &transform, renderer::vec4 const& translation) {
    std::array<int, 16> resultsMatrix;
    std::copy(transform.begin(), transform.end(), resultsMatrix.begin());
    
    resultsMatrix[3] += translation.x;
    resultsMatrix[7] += translation.y;
    resultsMatrix[11] += translation.z;

    return resultsMatrix;
}


//given an existing transform matrix, a normailised direction vector and a normalised axis vector rotate towards the diretion on the given axis
std::array<int, 16> applyRotation(std::array<float, 16> const& transform, renderer::vec4 const& direction, renderer::vec4 const& axis) {
    float32x4_t d = vld1q_f32(&direction);
    float32x4_t a = vld1q_f32(&axis);
    float32x4_t dot = vmovq_n_f32(0);

    dot = vmulq_f32(d, a);
    float c = vaddvq_f32(dot);
    
    //cross product   
    //bit masks for swizzling 
    static const uint8_t mask1_data[16] = {4,5,6,7, 8,9,10,11, 0,1,2,3, 12,13,14,15};    // [1, 2, 0, 3]
    static const uint8_t mask2_data[16] = {8,9,10,11, 0,1,2,3, 4,5,6,7, 12,13,14,15};    // [2, 0, 1, 3]

    uint8x16_t mask1 = vld1q_u8(mask1_data);
    uint8x16_t mask2 = vld1q_u8(mask2_data);

    uint8x16_t d_bytes = vreinterpretq_u8_f32(d);
    uint8x16_t a_bytes = vreinterpretq_u8_f32(a);

    float32x4_t d_zxy = vreinterpretq_f32_u8(vqtbl1q_u8(d_bytes, mask2));
    float32x4_t a_yzx = vreinterpretq_f32_u8(vqtbl1q_u8(a_bytes, mask1));

    float32x4_t b_yzx = vreinterpretq_f32_u8(vqtbl1q_u8(d_bytes, mask1));
    float32x4_t a_zxy = vreinterpretq_f32_u8(vqtbl1q_u8(a_bytes, mask2));

    float32x4_t prod = vmulq_f32(a_yzx, d_zxy);
    prod = vmlsq_f32(prod, a_zxy, b_yzx);

    vec4 v;// = cross( z, d );
    vst1q_f32(&v, prod);
    
    const float k = 1.0f/(1.0f+c);

    std::array<float,16> rotation = 
    { v.x*v.x*k + c,     v.y*v.x*k - v.z,    v.z*v.x*k + v.y, 0,
    v.x*v.y*k + v.z,   v.y*v.y*k + c,      v.z*v.y*k - v.x, 0,
    v.x*v.z*k - v.y,   v.y*v.z*k + v.x,    v.z*v.z*k + c, 0,
    0, 0, 0, 0 };

    //multiply rotation matrix in
    std::array<int, 16> resultMatrix;

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
std::array<int, 16>  applyScaling(std::array<float, 16> const& transform, float const &scalar) {
    applyScaling(transform, scalar, scalar, scalar);
};

//given an existing transform matrix and scaling factor per component apply the scaling
std::array<int, 16>  applyScaling(std::array<float, 16> const& transform, float const &scalarX, float const &scalarY, float const &scalarZ) {
    std::array<int, 16> resultMatrix;
    
    std::array<int, 16> scaleMatrix;
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
    float32x4_t vec = vld1q_f32(&v);
    float32x4_t temp = vmovq_n_f32(0);

    temp = vmulq_f32(vec, vec);
    float sum = vaddvq_f32(temp);
    sum = std::sqrt(sum);

    temp = vmovq_n_f32(sum);
    temp = vdivq_f32(vec, temp);
    vst1q_f32(&result, temp);
    return result;
};

std::array<float, 16> getCameraTransform(renderer::vec4 const& cameraPos);

renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform, renderer::AlignedVec4 const& verticies){
    renderer::AlignedVec4 worldVerticies;
    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    

    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12);

    float32x4_t vec;
    float32x4_t acc;
    int i = 0;
    for (auto it = verticies.begin(); it != verticies.end(); ++it) {
        vec = vld1q_f32(&*it);
        acc = vmovq_n_f32(0);
        acc = vfmaq_laneq_f32(acc, transformC0, vec, 0);
        acc = vfmaq_laneq_f32(acc, transformC1, vec, 1);
        acc = vfmaq_laneq_f32(acc, transformC2, vec, 2);
        acc = vfmaq_laneq_f32(acc, transformC3, vec, 3);
        vst1q_f32(worldVerticies.data() + i*16, acc);

        ++i;
    }

    return worldVerticies;
};

}