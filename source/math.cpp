#include <renderer/math.hpp>
#include <arm_neon.h>
#include <cmath>
#include <iostream>
#include <numbers>

namespace renderer {

std::array<float, 16> applyTranslation(std::array<float, 16> const& transform, renderer::vec4 const translation) {
    std::array<float, 16> resultsMatrix;
    std::copy(transform.begin(), transform.end(), resultsMatrix.begin());
    
    //This one is pretty simple we just add our translation values directly
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

    //transform matrix
    float32x4_t transformC0;
    float32x4_t transformC1;
    float32x4_t transformC2;
    float32x4_t transformC3;
    
    //rotation matrix
    float32x4_t rotationC0;
    float32x4_t rotationC1;
    float32x4_t rotationC2;
    float32x4_t rotationC3;

    //result matrix
    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;

    //load transorm matrix into 4 registers
    transformC0 = vld1q_f32(transform.data());
    transformC1 = vld1q_f32(transform.data()+4);
    transformC2 = vld1q_f32(transform.data()+8);
    transformC3 = vld1q_f32(transform.data()+12); 

    //construct result matrix
    resultC0 = vmovq_n_f32(0);
    resultC1 = vmovq_n_f32(0);
    resultC2 = vmovq_n_f32(0);
    resultC3 = vmovq_n_f32(0);

    //load first row of rotation matrix and multiply in
    rotationC0 = vld1q_f32(rotation.data());
    resultC0 = vfmaq_laneq_f32(resultC0, transformC0, rotationC0, 0); // result c0 = {t00*r00, t01 * r00, t02 * r00, t03 * r00}
    resultC0 = vfmaq_laneq_f32(resultC0, transformC1, rotationC0, 1); // result c0 = {t00*r00 + t10*r01, t01 * r00 + t11 * r01, t02 * r00 + t12 * r01, t03 * r00 + t13 * 01}
    resultC0 = vfmaq_laneq_f32(resultC0, transformC2, rotationC0, 2); //you get the point...
    resultC0 = vfmaq_laneq_f32(resultC0, transformC3, rotationC0, 3); //I ain't typing all that
    vst1q_f32(resultMatrix.data(), resultC0);

    //load first row of rotation matrix and multiply in
    rotationC1 = vld1q_f32(rotation.data()+4);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC0, rotationC1, 0);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC1, rotationC1, 1);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC2, rotationC1, 2);
    resultC1 = vfmaq_laneq_f32(resultC1, transformC3, rotationC1, 3);
    vst1q_f32(resultMatrix.data()+4, resultC1);

    //load first row of rotation matrix and multiply in
    rotationC2 = vld1q_f32(rotation.data()+8);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC0, rotationC2, 0);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC1, rotationC2, 1);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC2, rotationC2, 2);
    resultC2 = vfmaq_laneq_f32(resultC2, transformC3, rotationC2, 3);
    vst1q_f32(resultMatrix.data()+8, resultC2);

    //load first row of rotation matrix and multiply in
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

    //If you want comments check the x rotation, I'm not typing that again
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

    //if you really need comments check the x rotation, Yes I'm aware I could have made this a function
    //I wrote this out every single time because I wanted to drill in the simd muscle memory
    //if you factored out your working on math homework you got less marks for a reason
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

//given an existing transform matrix and scaling factor apply the scaling uniformly
std::array<float, 16>  applyScaling(std::array<float, 16> const& transform, float const scalar) {
    return applyScaling(transform, scalar, scalar, scalar);
};

//given an existing transform matrix and scaling factor per component apply the scaling
std::array<float, 16>  applyScaling(std::array<float, 16> const& transform, float const scalarX, float const scalarY, float const scalarZ) {
    std::array<float, 16> resultMatrix;
    
    std::array<float, 16> scaleMatrix;
    scaleMatrix[0] = scalarX;
    scaleMatrix[5] = scalarY;
    scaleMatrix[10] = scalarZ;
    scaleMatrix[15] = 1.0;


    //Yes I did write out the whole matrix multiplication, see applyRotationZ if you must know why
    // I know that this is very unnecessary
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

renderer::vec4 normalisedVector(renderer::vec4 const v) {
    renderer::vec4 result;
    float32x4_t vec = vld1q_f32(&v.x);
    float32x4_t temp = vmovq_n_f32(0);

    //getting the magnitude of the vector
    temp = vmulq_f32(vec, vec);
    float sum = vaddvq_f32(temp);
    sum = std::sqrt(sum);

    //divide throught the magnitude
    temp = vmovq_n_f32(sum);
    temp = vdivq_f32(vec, temp);
    vst1q_f32(&result.x, temp);
    return result;
};

renderer::AlignedVec4 localToTransform(std::array<float, 16> const& transform, renderer::AlignedVec4 const& verticies){
    renderer::AlignedVec4 worldVerticies(verticies.size(), {0, 0, 0, 0});
    
    //loading our uniform transform matrix
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
        //load 4 verticies
        float32x4x4_t v = vld1q_f32_x4(reinterpret_cast<float const*>(verticies.data()) + (i*4));
        
        resultC0 = vmovq_n_f32(0);
        resultC1 = vmovq_n_f32(0);
        resultC2 = vmovq_n_f32(0);
        resultC3 = vmovq_n_f32(0);

        //this is just doing the vector transforms as if they're a 4x4 matrix so we can do 4 at once
        //yes that means I did write out the matrix multiplication again
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

//a helper function for dividing the perspective transformed matrix by w
renderer::AlignedVec4 divideW(renderer::AlignedVec4 const& cartesianVerticies) {
    renderer::AlignedVec4 homogenousVerticies(cartesianVerticies.size(), {0, 0, 0, 0});
    
    float32x4_t resultC0;
    float32x4_t resultC1;
    float32x4_t resultC2;
    float32x4_t resultC3;
    
    size_t i = 0;
    size_t count = cartesianVerticies.size();
    for (;i < (count & ~3); i += 4) {
        //loading 4 vectors
        float32x4x4_t v = vld1q_f32_x4(reinterpret_cast<float const*>(cartesianVerticies.data()) + (i*4));
        
        //load w into a register for each vector
        resultC0 = vmovq_n_f32(v.val[0][3]);
        resultC1 = vmovq_n_f32(v.val[1][3]);
        resultC2 = vmovq_n_f32(v.val[2][3]);
        resultC3 = vmovq_n_f32(v.val[3][3]);

        //set the value in the last lane to 1 so we get to keep out w value unchanged
        resultC0 = vsetq_lane_f32(1, resultC0, 3);
        resultC1 = vsetq_lane_f32(1, resultC1, 3);
        resultC2 = vsetq_lane_f32(1, resultC2, 3);
        resultC3 = vsetq_lane_f32(1, resultC3, 3);        

        resultC0 = vdivq_f32(v.val[0], resultC0);
        resultC1 = vdivq_f32(v.val[1], resultC1);
        resultC2 = vdivq_f32(v.val[2], resultC2);
        resultC3 = vdivq_f32(v.val[3], resultC3);
        
        //store the reults
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

renderer::AlignedVec4 getProjectedCoordinates(AlignedVec4 const& verticies, vec4 const cameraPos, float const fov, float const near, float const far, float const aspectRatio) {
    vec4 cameraOffset = {cameraPos.x * -1,cameraPos.y * -1, cameraPos.z * -1, 1.0};
    
    std::array<float, 16> cameraTransform = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    //world to camera space
    cameraTransform = applyTranslation(cameraTransform, cameraOffset);
    AlignedVec4 cameraSpaceVerticies = localToTransform(cameraTransform, verticies);

    //perpective matrix construction
    const float top = tan(((fov/2)*(std::numbers::pi /180)))*near;
    const float bottom = -top;
    const float right = top * aspectRatio;
    const float left = -right;

    //I used the openGL perspective matrix
    //for a really great explanation of the math behind this matrix I recommend checking the readme
    //and reading the Scratch a pixel chapter on the perspective matrix 
    std::array<float, 16> perspectiveTransform = {
        2*near/(right-left), 0, 0, 0,
        0, 2*near/(top-bottom), 0, 0,
        0, 0, -((far + near)/(far - near)), -1,
        0, 0, -(2*far*near/(far - near)), 0
    };

    //camera space to NDC/clip space
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
        //loads the verticies with deinterleaving
        float32x4x4_t v = vld4q_f32(reinterpret_cast<float const*>(verticies.data()) + (i*4));
        
        // x' = (x + 1)/2 * width
        resultC0 = vaddq_f32(v.val[0], tempC0);
        resultC0 = vdivq_f32(resultC0, tempC1);
        resultC0 = vmulq_f32(resultC0, tempC2);

        // y' = (1 - y)/2 * height
        //we use -y since NDC positive y goes up but for raster coords positive y is down
        resultC1 = vsubq_f32(tempC0, v.val[1]);
        resultC1 = vdivq_f32(resultC1, tempC1);
        resultC1 = vmulq_f32(resultC1, tempC3);

        //raster cord = {x', y', z, w}
        float32x4x4_t v_r = {resultC0, resultC1, v.val[2], v.val[3]};
        vst4q_f32(reinterpret_cast<float *>(rasterVerts.data()) + (i*4), v_r);
    }

    //for this one since we'd essentially be doing every element individually there isn't really a reason to do this cleanup with simd
    for(;i<count;++i){
        rasterVerts[i] = {(verticies[i].x + 1)/2 * width, (1 - verticies[i].y)/2 * height, verticies[i].z, verticies[i].w};
    }

    return rasterVerts;
};

}