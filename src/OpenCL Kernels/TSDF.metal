//
//  TSDF.metal
//  RGBDLib
//
//  Created by Diego Thomas on 2018/07/18.
//  Copyright © 2018 3DLab. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;


struct Uniforms{
    float4x4 projectionMatrix;
    float Param[6];
    float nu;
};

struct Quaternion {
    float4 value;
    
    Quaternion(float X, float Y, float Z, float W) {
        value.x = X;
        value.y = Y;
        value.z = Z;
        value.w = W;
    }
    
    Quaternion (float3 t, float W) {
        value.x = t.x;
        value.y = t.y;
        value.z = t.z;
        value.w = W;
    }
    
    float3 Vector () {
        return value.xyz;
    }
    
    float Scalar () {
        return value.w;
    }
    
    // Scalar Multiplication
    Quaternion operator* (float s) {
        float3 v1 = value.xyz;
        return Quaternion (s*v1, s*value.w);
    }
    
    // Multiplication
    Quaternion operator* (Quaternion b) {
        float3 v1 = value.xyz;
        float w1 = value.w;
        float3 v2 = b.Vector();
        float w2 = b.Scalar();
        
        return Quaternion (w1*v2 + w2*v1 + cross(v1,v2), w1*w2 - dot(v1,v2));
    }
    
    // Addition
    Quaternion operator+ (Quaternion b) {
        float3 v1 = value.xyz;
        float w1 = value.w;
        float3 v2 = b.Vector();
        float w2 = b.Scalar();
        
        return Quaternion (v1 + v2, w1 + w2);
    }
    
    // Conjugate
    Quaternion Conjugate () {
        return Quaternion(-value.x, -value.y, -value.z, value.w);
    }
    
    // Magnitude
    float Magnitude () {
        return sqrt(value.x*value.x + value.y*value.y + value.z*value.z + value.w*value.w);
    }
    
    float Dot(Quaternion b) {
        float3 v1 = value.xyz;
        float w1 = value.w;
        float3 v2 = b.Vector();
        float w2 = b.Scalar();
        
        return w1*w2 + dot(v1,v2);
    }
    
    //Normalize
    Quaternion Normalize ( ) {
        float norm = sqrt(value.x*value.x + value.y*value.y + value.z*value.z + value.w*value.w);
        return Quaternion(value.x/norm, value.y/norm, value.z/norm, value.w/norm);
    }
};

struct DualQuaternion {
    Quaternion m_real = Quaternion(0.0,0.0,0.0,1.0);
    Quaternion m_dual = Quaternion(0.0,0.0,0.0,0.0);
    
    DualQuaternion(Quaternion r, Quaternion d) {
        m_real = r.Normalize();
        m_dual = d;
    }
    
    DualQuaternion(Quaternion r, float3 t) {
        m_real = r.Normalize();
        m_dual = (Quaternion(t,0.0) * m_real) * 0.5;
    }
    
    Quaternion Real() {
        return m_real;
    }
    
    Quaternion Dual () {
        return m_dual;
    }
    
    DualQuaternion Identity() {
        return DualQuaternion(Quaternion(float3(0.0f), 1.0f), Quaternion(float3(0.0f), 0.0f));
    }
    
    //Addition
    DualQuaternion operator+ (DualQuaternion b) {
        return DualQuaternion(m_real + b.Real(), m_dual + b.Dual());
    }
    
    // Scalar multiplication
    DualQuaternion operator* (float s) {
        return DualQuaternion(m_real*s, m_dual*s);
    }
    
    // Multiplication
    DualQuaternion operator* (DualQuaternion b) {
        return DualQuaternion(m_real*b.Real(), m_real*b.Dual() + m_dual*b.Real());
    }
    
    // Conjugate
    DualQuaternion Conjugate () {
        return DualQuaternion (m_real.Conjugate(), m_dual.Conjugate());
    }
    
    float Dot (DualQuaternion b) {
        return m_real.Dot(b.Real());
    }
    
    // Magnitude
    float Magnitude () {
        return m_real.Dot(m_real);
    }

    DualQuaternion Normalize () { // Here m_real.Dot(m_dual) should be = 0
        float mag = m_real.Dot(m_real);
        if (mag < 0.000001f)
            return DualQuaternion(Quaternion(0.0, 0.0, 0.0, 0.0), Quaternion(0.0, 0.0, 0.0, 0.0));
        return DualQuaternion(m_real*(1.0f/mag), m_dual*(1.0/mag));
    }
    
    Quaternion GetRotation () {
        return m_real;
    }
    
    float3 GetTranslation () {
        Quaternion t = (m_dual * 2.0f) * m_real.Conjugate();
        return t.Vector();
    }
    
    float4x4 DualQuaternionToMatrix () {
        float4x4 M;
        
        float mag = m_real.Dot(m_real);
        if (mag < 0.000001f)
            return M;
        DualQuaternion q = DualQuaternion(m_real*(1.0f/mag), m_dual*(1.0/mag));
        
        float w = q.m_real.Scalar();
        float3 v = q.m_real.Vector();
        
        M[0][0] = w*w + v.x*v.x - v.y*v.y - v.z*v.z;
        M[1][0] = 2.0f*v.x*v.y + 2.0f*w*v.z;
        M[2][0] = 2.0f*v.x*v.z - 2.0f*w*v.y;
        M[3][0] = 0.0f;
        
        M[0][1] = 2.0f*v.x*v.y - 2.0f*w*v.z;
        M[1][1] = w*w + v.y*v.y - v.x*v.x - v.z*v.z;
        M[2][1] = 2.0f*v.y*v.z + 2.0f*w*v.x;
        M[3][1] = 0.0f;
        
        M[0][2] = 2.0f*v.x*v.z + 2.0f*w*v.y;
        M[1][2] = 2.0f*v.y*v.z - 2.0f*w*v.x;
        M[2][2] = w*w + v.z*v.z - v.x*v.x - v.y*v.y;
        M[3][2] = 0.0f;
        
        Quaternion t = (m_dual * 2.0f) * m_real.Conjugate();
        float3 t_v = t.Vector();
        M[0][3] = t_v.x;
        M[1][3] = t_v.y;
        M[2][3] = t_v.z;
        M[3][3] = 1.0f;
        
        return M;
    }
};

// Init kernel
kernel void initTSDF(   device float2 *TSDF [[ buffer(0) ]],
                        device float2 *Field [[ buffer(1) ]],
                        constant int *size [[ buffer(2) ]],
                     uint2 threadgroup_position_in_grid   [[ threadgroup_position_in_grid ]],
                     uint2 thread_position_in_threadgroup [[ thread_position_in_threadgroup ]],
                     uint2 threads_per_threadgroup        [[ threads_per_threadgroup ]]) {
    // Ensure we don't read or write outside of the texture
    int h = threadgroup_position_in_grid.x * (threads_per_threadgroup.x * threads_per_threadgroup.y) + thread_position_in_threadgroup.x*threads_per_threadgroup.y + thread_position_in_threadgroup.y;
    int k = threadgroup_position_in_grid.y;
    
    int i = (int)(h/size[1]);
    int j = h - i*size[1];
    
    if (i >= size[0] || j >= size[1] || k >= size[2])
        return;
    
    int indx = h*size[2] + k;
    TSDF[indx].x = -1.0;
    TSDF[indx].y = 0.0;
}

DualQuaternion DQB(constant DualQuaternion *Warp, constant float2 *Field) {
    DualQuaternion q = DualQuaternion(Quaternion(float3(0.0f), 0.0f), Quaternion(float3(0.0f), 0.0f));
    float weight_T = 0.0f;
    
    // Blend all dual quaternions
    
    for (int i = 0; i < 5; i++) { // max 5 atached nodes to a single vertex
        if (Field[i].x == -1.0f)
            break;
        DualQuaternion curr = Warp[int(Field[i].x)];
        float weight = Field[i].y;
        
        q = q*weight_T+curr*weight;
        weight_T += weight;
    }
    
    q = q * (1.0f/weight_T);
    return q.Normalize();
}

// Update kernel
kernel void UpdateTSDF(   device float2 *TSDF [[ buffer(0) ]],
                       constant DualQuaternion *Warp [[ buffer(1) ]],
                       constant float2 *Field [[ buffer(2) ]],
                       const device Uniforms& uniforms[[buffer(3)]],
                       constant int *size[[buffer(4)]],
                       texture2d<float, access::read>  inputTexture0      [[ texture(0) ]],
                       uint2 threadgroup_position_in_grid   [[ threadgroup_position_in_grid ]],
                       uint2 thread_position_in_threadgroup [[ thread_position_in_threadgroup ]],
                       uint2 threads_per_threadgroup        [[ threads_per_threadgroup ]]) {
    // Ensure we don't read or write outside of the texture
    int h = threadgroup_position_in_grid.x * (threads_per_threadgroup.x * threads_per_threadgroup.y) + thread_position_in_threadgroup.x*threads_per_threadgroup.y + thread_position_in_threadgroup.y;
    int k = threadgroup_position_in_grid.y;
    
    int i = (int)(h/size[1]);
    int j = h - i*size[1];
    
    if (i >= size[0] || j >= size[1] || k >= size[2])
        return;
    
    int indx = h*size[2] + k;
    float4 vtx = float4(1.0f);
    vtx.x = ((float)(i) - uniforms.Param[0])/uniforms.Param[1];
    vtx.y = ((float)(j) - uniforms.Param[2])/uniforms.Param[3];
    vtx.z = -0.5f-((float)(k) - uniforms.Param[4])/uniforms.Param[5];
    
    if (vtx.z == 0.0)
        return;
        
    //DualQuaternion p = DualQuaternion(Quaternion(float3(0.0f), 1.0f), Quaternion(vtx.xyz, 0.0));
    
    // Get blended transformation for the current voxel
    //DualQuaternion pose = DQB(Warp, &Field[5*indx]);
    
    //DualQuaternion pT = pose*p*pose.Conjugate();
    
    float depth = vtx.z;
    vtx.z = fabs(vtx.z);
    
    float4 new_vtx = uniforms.projectionMatrix*vtx; //float4(pT.Dual().Vector(), 1.0f);

    //Transform
    //float4 new_vtx = projectionMatrix*pose*vtx;
    
    //Project
    int ix = round(new_vtx.x/new_vtx.z);
    int jx = round(inputTexture0.get_height() - new_vtx.y/new_vtx.z);
    
    new_vtx.z = depth;
    
    // Ensure we don't read or write outside of the texture
    // should be: x between 0 and 640; y beetween 0 and 480
    if (ix < 0 || (ix >= int(inputTexture0.get_width())) || jx < 0 || (jx >= int(inputTexture0.get_height())) )
        return;
    
    float4 vtxIn = inputTexture0.read(uint2(ix,jx));
    
    if (vtxIn.x == 0.0f && vtxIn.y == 0.0f && vtxIn.z == 0.0f)
        return;
    
    // Update TSDF value
    float tsdf_val = min(1.0f, max(-1.0f, (new_vtx.z - vtxIn.z)/uniforms.nu));
    
    TSDF[indx].x = (TSDF[indx].x*TSDF[indx].y + tsdf_val)/(TSDF[indx].y+1.0f);
    TSDF[indx].y = TSDF[indx].y+1.0f;
}

// Update Warp field kernel
kernel void UpdateFieldTSDF(   device float2 *TSDF [[ buffer(0) ]],
                       constant float4 *Warp [[ buffer(1) ]],
                       constant float2 *Field [[ buffer(2) ]],
                       constant Uniforms& uniforms[[buffer(3)]],
                       constant int *size[[buffer(4)]],
                       texture2d<float, access::read>  inputTexture0      [[ texture(0) ]],
                       uint2 gid [[ thread_position_in_grid ]]) {
    // Ensure we don't read or write outside of the texture
    int i = gid.x;
    int j = gid.y;
    if (i >= size[0] || j >= size[1])
        return;
}
