//
//  MarchingCubes.metal
//  RGBDLib
//
//  Created by Diego Thomas on 2018/07/19.
//  Copyright © 2018 3DLab. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

//    create the precalculated 256 possible polygon configuration (128 + symmetries)
constant int Config[128][4][3] = { {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}, //0
    {{0, 7, 3}, {0,0,0}, {0,0,0}, {0,0,0}}, // v0 1
    {{0, 1, 4}, {0,0,0}, {0,0,0}, {0,0,0}}, // v1 2
    {{1, 7, 3}, {1, 4, 7}, {0,0,0}, {0,0,0}}, // v0|v1 3
    {{1, 2, 5}, {0,0,0}, {0,0,0}, {0,0,0}}, // v2 4
    {{0, 7, 3}, {1, 2, 5}, {0,0,0}, {0,0,0}}, //v0|v2 5
    {{0, 2 ,4}, {2, 5, 4}, {0,0,0}, {0,0,0}}, //v1|v2 6
    {{3, 2, 7}, {2, 5, 7}, {5, 4, 7}, {0,0,0}}, //v0|v1|v2 7
    {{2, 3, 6}, {0,0,0}, {0,0,0}, {0,0,0}}, // v3 8
    {{0, 7, 2}, {7, 6, 2}, {0,0,0}, {0,0,0}}, //v0|v3 9
    {{0, 1, 4}, {3, 6, 2}, {0,0,0}, {0,0,0}}, //v1|v3 10
    {{1, 4, 2}, {2, 4, 6}, {4, 7, 6}, {0,0,0}}, //v0|v1|v3 11
    {{3, 5, 1}, {3, 6, 5}, {0,0,0}, {0,0,0}}, //v2|v3 12
    {{0, 7, 1}, {1, 7, 5}, {7, 6, 5}, {0,0,0}}, //v0|v2|v3 13
    {{0, 3, 4}, {3, 6, 4}, {6, 5, 4}, {0,0,0}}, //v1|v2|v3 14
    {{7, 6, 5}, {4, 7, 5}, {0,0,0}, {0,0,0}}, //v0|v1|v2|v3 15
    {{7, 8, 11}, {0,0,0}, {0,0,0}, {0,0,0}}, // v4 16
    {{0, 8, 3}, {3, 8, 11}, {0,0,0}, {0,0,0}}, //v0|v4 17
    {{0, 1, 4}, {7, 8, 11}, {0,0,0}, {0,0,0}}, //v1|v4 18
    {{1, 4, 8}, {1, 8, 11}, {1, 11, 3}, {0,0,0}}, //v0|v1|v4 19
    {{7, 8, 11}, {1, 2, 5}, {0,0,0}, {0,0,0}}, //v2|v4 20
    {{0, 8, 3}, {3, 8, 11}, {1, 2, 5}, {0,0,0}}, //v0|v2|v4 21
    {{0, 2 ,4}, {2, 5, 4}, {7, 8, 11}, {0,0,0}}, //v1|v2|v4 22
    {{4, 2, 5}, {2, 4, 11}, {4, 8, 11}, {2, 11, 3}}, //v0|v1|v2|v4 23
    {{2, 3, 6}, {7, 8, 11}, {0,0,0}, {0,0,0}}, //v3|v4 24
    {{6, 8, 11}, {2, 8, 6}, {0, 8, 2}, {0,0,0}}, //v0|v3|v4 25
    {{0, 1, 4}, {2, 3, 6}, {7, 8, 11}, {0,0,0}}, //v1|v3|v4 26
    {{6, 8, 11}, {8, 6, 2}, {4, 8, 2}, {4, 2, 1}}, //v0|v1|v3|v4 27
    {{7, 8, 11}, {3, 5, 1}, {3, 6, 5}, {0,0,0}}, //v2|v3|v4 28
    {{0, 8 ,1}, {1, 8, 6}, {1, 6, 5}, {6, 8, 11}}, //v0|v2|v3|v4 29
    {{7, 8, 11}, {0, 3, 4}, {3, 6, 4}, {6, 5, 4}}, //v1|v2|v3|v4 30
    {{6, 8, 11}, {6, 4, 8}, {5, 4, 6}, {0,0,0}}, //v0|v1|v2|v3|v4 31 =                              v5|v6|v7   //////////////////////////
    {{4, 9, 8}, {0,0,0}, {0,0,0}, {0,0,0}}, // v5 32
    {{0, 7, 3}, {4, 9, 8}, {0,0,0}, {0,0,0}}, //v0|v5 33
    {{0, 1, 8}, {1, 9, 8}, {0,0,0}, {0,0,0}}, //v1|v5 34
    {{1, 9, 3}, {3, 9, 7}, {7, 9, 8}, {0,0,0}}, //v0|v1|v5 35
    {{4, 9, 8}, {1, 2, 5}, {0,0,0}, {0,0,0}}, //v2|v5 36
    {{4, 9, 8}, {1, 2, 5}, {0, 7, 3}, {0,0,0}}, //v0|v2|v5 37
    {{0, 2 ,8}, {2, 5, 8}, {8, 5, 9}, {0,0,0}}, //v1|v2|v5 38
    {{7, 9, 8}, {3, 9, 7}, {3, 5, 9}, {2, 5, 3}}, //v0|v1|v2|v5 39
    {{4, 9, 8}, {2, 3, 6}, {0,0,0}, {0,0,0}}, //v3|v5 40
    {{4, 9, 8}, {0, 7, 2}, {7, 6, 2}, {0,0,0}}, //v0|v3|v5 41
    {{2, 3, 6}, {0, 1, 8}, {1, 9, 8}, {0,0,0}}, //v1|v3|v5 42
    {{1, 9, 2}, {2, 9, 7}, {7, 6, 9}, {7, 9, 8}}, //v0|v1|v3|v5 43
    {{4, 9, 8}, {3, 5, 1}, {3, 6, 5}, {0,0,0}}, //v2|v3|v5 44
    {{4, 9, 8}, {0, 7, 1}, {1, 7, 5}, {7, 6, 5}}, //v0|v2|v3|v5 45
    {{5, 9, 8}, {0, 3, 8}, {3, 5, 8}, {3, 6, 5}}, //v1|v2|v3|v5 46
    {{5, 7, 6}, {8, 5, 9}, {7, 5, 8}, {0,0,0}}, //v0|v1|v2|v3|v5 47                                     = v4 | v6 | v7  ////////////////////
    {{4, 9, 7}, {7, 9, 11}, {0,0,0}, {0,0,0}}, //v4|v5 48
    {{3, 9, 11}, {0, 4, 3}, {4, 9, 3}, {0,0,0}}, //v0|v4|v5 49
    {{1, 9, 11}, {0, 11, 7}, {0, 1, 11}, {0,0,0}}, //v1|v4|v5 50
    {{1, 9, 11}, {1, 11, 3}, {0,0,0}, {0,0,0}}, //v0|v1|v4|v5 51
    {{1, 2, 5}, {4, 9, 7}, {7, 9, 11}, {0,0,0}}, //v2|v4|v5 52
    {{1, 2, 5}, {3, 9, 11}, {0, 4, 3}, {4, 9, 3}}, //v0|v2|v4|v5 53
    {{0, 2, 7}, {2, 5, 9}, {2, 9, 7}, {7, 9, 11}}, //v1|v2|v4|v5 54
    {{11, 3, 9}, {3, 2, 5}, {9, 3, 5}, {0,0,0}}, //v0|v1|v2|v4|v5 55                                          = v3 v6 v7 //////////////////////////
    {{2, 3, 6}, {4, 9, 7}, {7, 9, 11}, {0,0,0}}, //v3|v4|v5 56
    {{2, 0, 4}, {6, 2, 11}, {2, 4, 11}, {4, 9, 11}}, //v0|v3|v4|v5 57
    {{2, 3, 6}, {1, 2, 5}, {4, 9, 7}, {7, 9, 11}}, //v1|v3|v4|v5 58
    {{11, 1, 9}, {2, 1, 6}, {6, 1, 11}, {0,0,0}}, //v0|v1|v3|v4|v5 59                                                               = v2 v6 v7 //////////////////
    {{7, 4, 11}, {4, 9, 11}, {3, 6, 1}, {1, 6, 5}}, //v2|v3|v4|v5 60
    {{1, 0, 4}, {11, 6, 9}, {9, 6, 5}, {0,0,0}}, //v0|v2|v3|v4|v5 61                                                                  = v1 v6 v7
    {{3, 0, 7}, {11, 6, 9}, {9, 6, 5}, {0,0,0}}, //v1|v2|v3|v4|v5 62                                                                 = v0 v6 v7
    {{11, 6, 9}, {9, 6, 5}, {0,0,0}, {0,0,0}}, //v0|v1|v2|v3|v4|v5 63                                                                 = v6 v7
    {{5, 10, 9}, {0,0,0}, {0,0,0}, {0,0,0}}, //v6 64
    {{5, 10, 9}, {0, 7, 3}, {0,0,0}, {0,0,0}}, //v0|v6 65
    {{5, 10, 9}, {0, 1, 4}, {0,0,0}, {0,0,0}}, //v1|v6 66
    {{5, 10, 9}, {1, 3, 7}, {1, 7, 4}, {0,0,0}}, //v0|v1|v6 67
    {{1, 2, 9}, {2, 10, 9}, {0,0,0}, {0,0,0}}, //v2|v6 68
    {{1, 2, 9}, {2, 10, 9}, {0, 7, 3}, {0,0,0}}, //v0|v2|v6 69
    {{0, 2, 10}, {4, 10, 9}, {0, 10, 4}, {0,0,0}}, //v1|v2|v6 70
    {{2, 10, 3}, {4, 10, 9}, {4, 3, 10}, {3, 4, 7}}, //v0|v1|v2|v6 71
    {{5, 10, 9}, {2, 3, 6}, {0,0,0}, {0,0,0}}, //v3|v6 72
    {{5, 10, 9}, {0, 7, 2}, {7, 6, 2}, {0,0,0}}, //v0|v3|v6 73
    {{5, 10, 9}, {0, 1, 4}, {2, 3, 6}, {0,0,0}}, //v1|v3|v6 74
    {{5, 10, 9}, {1, 4, 2}, {2, 4, 6}, {4, 6, 7}}, //v0|v1|v3|v6 75
    {{1, 3, 9}, {6, 10, 9}, {3, 6, 9}, {0,0,0}}, //v2|v3|v6 76
    {{0, 7, 6}, {6, 10, 9}, {0, 6, 9}, {0, 9, 1}}, //v0|v2|v3|v6 77
    {{6, 10, 9}, {3, 6, 9}, {3, 9, 4}, {0, 3, 4}}, //v1|v2|v3|v6 78
    {{4, 7, 6}, {4, 10, 9}, {4, 6, 10}, {0,0,0}}, //v0|v1|v2|v3|v6 79    v4 v5 v7   ////////////////////////////
    {{5, 10, 9}, {7, 8, 11}, {0,0,0}, {0,0,0}}, //v4|v6 80
    {{5, 10, 9}, {0, 8, 3}, {3, 8, 11}, {0,0,0}}, //v0|v4|v6 81
    {{0, 1, 4}, {7, 8, 11}, {5, 10, 9}, {0,0,0}}, //v1|v4|v6 82
    {{5, 10, 9}, {1, 4, 8}, {1, 8, 11}, {1, 11, 3}}, //v0|v1|v4|v6 83
    {{1, 2, 9}, {2, 10, 9}, {7, 8, 11}, {0,0,0}}, //v2|v4|v6 84
    {{1, 2, 9}, {2, 10, 9}, {0, 8, 3}, {3, 8, 11}}, //v0|v2|v4|v6 85
    {{7, 8, 11}, {0, 2, 10}, {4, 10, 9}, {0, 10, 4}}, //v1|v2|v4|v6 86
    {{4, 8, 9}, {3, 2, 11}, {2, 10, 11}, {0,0,0}}, //v0|v1|v2|v4|v6 87                      = v3 v5 v7
    {{2, 3, 6}, {7, 8, 11}, {5, 10, 9}, {0,0,0}}, //v3|v4|v6 88
    {{5, 10, 9}, {6, 8, 11}, {2, 8, 6}, {0, 8, 2}}, //v0|v3|v4|v6 89
    {{0, 1, 4}, {2, 3, 6}, {7, 8, 11}, {5, 10, 9}}, //v1|v3|v4|v6 90
    {{2, 1, 5}, {9, 4, 8}, {11, 6, 10}, {0,0,0}}, //v0|v1|v3|v4|v6 91     = v2 v5 v7   //////////////////////
    {{7, 8, 11}, {1, 3, 9}, {6, 10, 9}, {3, 6, 9}}, //v2|v3|v4|v6 92
    {{11, 6, 10}, {1, 0, 8}, {9, 1, 8}, {0,0,0}}, //v0|v2|v3|v4|v6 93                              = v1 v5 v7 //////////
    {{0, 3, 7}, {9, 4, 8}, {10, 5, 9}, {0,0,0}}, //v1|v2|v3|v4|v6 94                                      = v0 v5 v7  ////////////
    {{11, 6, 10}, {9, 4, 8}, {0,0,0}, {0,0,0}}, //v0|v1|v2|v3|v4|v6 95                                               = v5 v7 ////////////
    {{4, 5, 8}, {8, 5, 10}, {0,0,0}, {0,0,0}}, //v5|v6 96
    {{0, 7, 3}, {4, 5, 8}, {8, 5, 10}, {0,0,0}}, //v0|v5|v6 97
    {{0, 10, 8}, {1, 5, 10}, {0, 1, 10}, {0,0,0}}, //v1|v5|v6 98
    {{1, 5, 10}, {8, 7, 10}, {1, 10, 7}, {3, 1, 7}}, //v0|v1|v5|v6 99
    {{2, 10, 8}, {1, 8, 4}, {1, 2, 8}, {0,0,0}}, //v2|v5|v6 100
    {{2, 10, 8}, {1, 8, 4}, {1, 2, 8}, {0, 7, 3}}, //v0|v2|v5|v6 101
    {{0, 10, 8}, {0, 2, 10}, {0,0,0}, {0,0,0}}, //v1|v2|v5|v6 102
    {{8, 2, 10}, {3, 8, 7}, {3, 2, 8}, {0,0,0}}, //v0|v1|v2|v5|v6 103                                                         = v3 v4 v7 //////////////
    {{2, 3, 6}, {4, 5, 8}, {8, 5, 10}, {0,0,0}}, //v3|v5|v6 104
    {{4, 5, 8}, {8, 5, 10}, {0, 7, 2}, {7, 6, 2}}, //v0|v3|v5|v6 105
    {{2, 3, 6}, {0, 10, 8}, {1, 5, 10}, {0, 1, 10}}, //v1|v3|v5|v6 106
    {{1, 5, 2}, {7, 6, 8}, {6, 10, 8}, {0,0,0}}, //v0|v1|v3|v5|v6 107                                                 =v2 v4 v7 ////////////////////////
    {{3, 6, 10}, {1, 3, 4}, {4, 3, 10}, {4, 10, 8}}, //v2|v3|v5|v6 108
    {{1, 0, 4}, {7, 6, 8}, {6, 10, 8}, {0,0,0}}, //v0|v2|v3|v5|v6 109                                                 = v1 v4 v7 //////////
    {{0, 10, 8}, {0, 3, 6}, {0, 6, 10}, {0,0,0}}, //v1|v2|v3|v5|v6 110                                                = v0 v4 v7 //////////
    {{7, 6, 8}, {6, 10, 8}, {0,0,0}, {0,0,0}}, //v0|v1|v2|v3|v5|v6 111                                                = v4 v7 //////////
    {{4, 5, 7}, {7, 10, 11}, {7, 5, 10}, {0,0,0}}, //v4|v5|v6 112
    {{5, 10, 11}, {0, 4, 5}, {0, 5, 11}, {0, 11, 3}}, //v0|v4|v5|v6 113
    {{0, 1, 5}, {0, 5, 10}, {0, 10, 7}, {7, 10, 11}}, //v1|v4|v5|v6 114
    {{3, 1, 11}, {1, 5, 10}, {1, 10, 11}, {0,0,0}}, //v0|v1|v4|v5|v6 115                                             = v2 v3 v7 ////////////
    {{7, 10, 11}, {4, 11, 7}, {4, 10, 11}, {1, 10, 4}}, //v2|v4|v5|v6 116
    {{0, 4, 1}, {3, 2, 11}, {2, 10, 11}, {0,0,0}}, //v0|v2|v4|v5|v6 117                                            = v1 v3 v7 ////////////
    {{0, 2, 10}, {0, 11, 7}, {0, 10, 11}, {0,0,0}}, //v1|v2|v4|v5|v6 118                                            = v0 v3 v7 ////////////
    {{3, 2, 11}, {2, 10, 11}, {0,0,0}, {0,0,0}}, //v0|v1|v2|v4|v5|v6 119                                           = v3 v7 ////////////
    {{2, 3, 6}, {4, 5, 7}, {7, 10, 11}, {7, 5, 10}}, //v3|v4|v5|v6 120
    {{6, 10, 11}, {0, 4, 2}, {2, 4, 5}, {0,0,0}}, //v0|v3|v4|v5|v6 121                                          = v1 v2 v7 ////////////
    {{3, 0, 7}, {2, 1, 5}, {11, 6, 10}, {0,0,0}}, //v1|v3|v4|v5|v6 122                                          = v0 v2 v7 ////////////
    {{2, 1, 5}, {11, 6, 10}, {0,0,0}, {0,0,0}}, //v0|v1|v3|v4|v5|v6 123                                          = v2 v7 ////////////
    {{3, 1, 7}, {7, 1, 4}, {11, 6, 10}, {0,0,0}}, //v2|v3|v4|v5|v6 124                                         = v0 v1 v7 ////////////
    {{1, 0, 4}, {11, 6, 10}, {0,0,0}, {0,0,0}}, //v0|v2|v3|v4|v5|v6 125                                         = v1 v7 ////////////
    {{3, 0, 7}, {11, 6, 10}, {0,0,0}, {0,0,0}}, //v1|v2|v3|v4|v5|v6 126                                         = v0 v7 ////////////
    {{11, 6, 10}, {0,0,0}, {0,0,0}, {0,0,0}} //v0|v1|v2|v3|v4|v5|v6 127                                         = v7 ////////////
};

constant int ConfigCount[128] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 1, 2, 2, 3,
    2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2, 3, 4, 4, 3, 3, 4, 4, 3, 4, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4,
    3, 2, 3, 3, 4, 3, 4, 4, 3, 3, 4, 4, 3, 4, 3, 3, 2, 2, 3, 3, 4, 3, 4, 2, 3, 3, 4, 4, 3, 4, 3, 3, 2, 3, 4, 4, 3, 4, 3, 3, 2, 4, 3,
    3, 2, 3, 2, 2, 1 };


kernel void MarchingCubes(   device float2 *TSDF [[ buffer(0) ]],
                             device int *Offset [[buffer(1)]],
                             device int *IndexVal [[buffer(2)]],
                             device float *Vertices [[buffer(3)]],
                             device float *Normales [[buffer(4)]],
                             device int *Faces [[buffer(5)]],
                             constant float *Param [[buffer(6)]],
                             constant int *Dim [[ buffer(7) ]],
                          uint2 threadgroup_position_in_grid   [[ threadgroup_position_in_grid ]],
                          uint2 thread_position_in_threadgroup [[ thread_position_in_threadgroup ]],
                          uint2 threads_per_threadgroup        [[ threads_per_threadgroup ]]) {
    
    int h = threadgroup_position_in_grid.x * (threads_per_threadgroup.x * threads_per_threadgroup.y) + thread_position_in_threadgroup.x*threads_per_threadgroup.y + thread_position_in_threadgroup.y;
    int z = threadgroup_position_in_grid.y;
    
    int x = (int)(h/Dim[1]);
    int y = h - x*Dim[1];
    
    if (x < 1 || x >= Dim[0]-2 || y < 1 || y >= Dim[1]-2 || z < 1 || z >= Dim[2]-2)
        return;
    
    int id = h*Dim[2] + z;
    int index = IndexVal[id];
    
    if (index == 0)
        return;
    
    float s[8][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}};
    
    
    float nmle[8][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}};
    
    float v[12][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}};
    
    float n[12][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
        {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}};
    
    float vals[8] = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    
    // get the 8  current summits
    s[0][0] = ((float)(x) - Param[0])/Param[1];
    s[0][1] = ((float)(y) - Param[2])/Param[3];
    s[1][0] = ((float)(x+1) - Param[0])/Param[1];
    s[1][1] = ((float)(y) - Param[2])/Param[3];
    s[2][0] = ((float)(x+1) - Param[0])/Param[1];
    s[2][1] = ((float)(y+1) - Param[2])/Param[3];
    s[3][0] = ((float)(x) - Param[0])/Param[1];
    s[3][1] = ((float)(y+1) - Param[2])/Param[3];
    s[4][0] = ((float)(x) - Param[0])/Param[1];
    s[4][1] = ((float)(y) - Param[2])/Param[3];
    s[5][0] = ((float)(x+1) - Param[0])/Param[1];
    s[5][1] = ((float)(y) - Param[2])/Param[3];
    s[6][0] = ((float)(x+1) - Param[0])/Param[1];
    s[6][1] = ((float)(y+1) - Param[2])/Param[3];
    s[7][0] = ((float)(x) - Param[0])/Param[1];
    s[7][1] = ((float)(y+1) - Param[2])/Param[3];
    
    // get the 8  current summits
    s[0][2] = -0.5f-((float)(z) - Param[4])/Param[5];
    s[1][2] = -0.5f-((float)(z) - Param[4])/Param[5];
    s[2][2] = -0.5f-((float)(z) - Param[4])/Param[5];
    s[3][2] = -0.5f-((float)(z) - Param[4])/Param[5];
    s[4][2] = -0.5f-((float)(z+1) - Param[4])/Param[5];
    s[5][2] = -0.5f-((float)(z+1) - Param[4])/Param[5];
    s[6][2] = -0.5f-((float)(z+1) - Param[4])/Param[5];
    s[7][2] = -0.5f-((float)(z+1) - Param[4])/Param[5];
    
    bool reverse = false;
    if (index < 0) {
        reverse = true;
        index = -index;
    }
        
    //convert TSDF to float
    float tsdf0 = Param[1]*0.05f*TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*x].x;
    float tsdf1 = Param[1]*0.05f*TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    float tsdf2 = Param[1]*0.05f*TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    float tsdf3 = Param[1]*0.05f*TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    float tsdf4 = Param[1]*0.05f*TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*x].x;
    float tsdf5 = Param[1]*0.05f*TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    float tsdf6 = Param[1]*0.05f*TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    float tsdf7 = Param[1]*0.05f*TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    
    // Compute normals at grid vertices
    nmle[0][0] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x-1)].x - TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    nmle[0][1] = TSDF[z + Dim[0]*(y-1) + Dim[0]*Dim[1]*x].x - TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    nmle[0][2] = TSDF[(z-1) + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[(z+1) + Dim[0]*y + Dim[0]*Dim[1]*x].x;
    float mag = sqrt(nmle[0][0]*nmle[0][0] + nmle[0][1]*nmle[0][1] + nmle[0][2]*nmle[0][2]);
    if (mag > 0.0f) {
        nmle[0][0] = nmle[0][0]/mag; nmle[0][1] = nmle[0][1]/mag; nmle[0][2] = nmle[0][2]/mag;
    }
    
    nmle[1][0] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+2)].x;
    nmle[1][1] = TSDF[z + Dim[0]*(y-1) + Dim[0]*Dim[1]*(x+1)].x - TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[1][2] = TSDF[(z-1) + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x - TSDF[(z+1) + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    mag = sqrt(nmle[1][0]*nmle[1][0] + nmle[1][1]*nmle[1][1] + nmle[1][2]*nmle[1][2]);
    if (mag > 0.0f) {
        nmle[1][0] = nmle[1][0]/mag; nmle[1][1] = nmle[1][1]/mag; nmle[1][2] = nmle[1][2]/mag;
    }
    
    nmle[2][0] = TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x - TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+2)].x;
    nmle[2][1] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x - TSDF[z + Dim[0]*(y+2) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[2][2] = TSDF[(z-1) + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x - TSDF[(z+1) + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    mag = sqrt(nmle[2][0]*nmle[2][0] + nmle[2][1]*nmle[2][1] + nmle[2][2]*nmle[2][2]);
    if (mag > 0.0f) {
        nmle[2][0] = nmle[2][0]/mag; nmle[2][1] = nmle[2][1]/mag; nmle[2][2] = nmle[2][2]/mag;
    }
    
    nmle[3][0] = TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x-1)].x - TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[3][1] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[z + Dim[0]*(y+2) + Dim[0]*Dim[1]*x].x;
    nmle[3][2] = TSDF[(z-1) + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x - TSDF[(z+1) + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    mag = sqrt(nmle[3][0]*nmle[3][0] + nmle[3][1]*nmle[3][1] + nmle[3][2]*nmle[3][2]);
    if (mag > 0.0f) {
        nmle[3][0] = nmle[3][0]/mag; nmle[3][1] = nmle[3][1]/mag; nmle[3][2] = nmle[3][2]/mag;
    }
    
    nmle[4][0] = TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x-1)].x - TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    nmle[4][1] = TSDF[z+1 + Dim[0]*(y-1) + Dim[0]*Dim[1]*x].x - TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    nmle[4][2] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[(z+2) + Dim[0]*y + Dim[0]*Dim[1]*x].x;
    mag = sqrt(nmle[4][0]*nmle[4][0] + nmle[4][1]*nmle[4][1] + nmle[4][2]*nmle[4][2]);
    if (mag > 0.0f) {
        nmle[4][0] = nmle[4][0]/mag; nmle[4][1] = nmle[4][1]/mag; nmle[4][2] = nmle[4][2]/mag;
    }
    
    nmle[5][0] = TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x+2)].x;
    nmle[5][1] = TSDF[z+1 + Dim[0]*(y-1) + Dim[0]*Dim[1]*(x+1)].x - TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[5][2] = TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x - TSDF[(z+2) + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x;
    mag = sqrt(nmle[5][0]*nmle[5][0] + nmle[5][1]*nmle[5][1] + nmle[5][2]*nmle[5][2]);
    if (mag > 0.0f) {
        nmle[5][0] = nmle[5][0]/mag; nmle[5][1] = nmle[5][1]/mag; nmle[5][2] = nmle[5][2]/mag;
    }
    
    nmle[6][0] = TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x - TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+2)].x;
    nmle[6][1] = TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x+1)].x - TSDF[z+1 + Dim[0]*(y+2) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[6][2] = TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x - TSDF[(z+2) + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    mag = sqrt(nmle[6][0]*nmle[6][0] + nmle[6][1]*nmle[6][1] + nmle[6][2]*nmle[6][2]);
    if (mag > 0.0f) {
        nmle[6][0] = nmle[6][0]/mag; nmle[6][1] = nmle[6][1]/mag; nmle[6][2] = nmle[6][2]/mag;
    }
    
    nmle[7][0] = TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x-1)].x - TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)].x;
    nmle[7][1] = TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*x].x - TSDF[z+1 + Dim[0]*(y+2) + Dim[0]*Dim[1]*x].x;
    nmle[7][2] = TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x - TSDF[(z+2) + Dim[0]*(y+1) + Dim[0]*Dim[1]*x].x;
    mag = sqrt(nmle[7][0]*nmle[7][0] + nmle[7][1]*nmle[7][1] + nmle[7][2]*nmle[7][2]);
    if (mag > 0.0f) {
        nmle[7][0] = nmle[7][0]/mag; nmle[7][1] = nmle[7][1]/mag; nmle[7][2] = nmle[7][2]/mag;
    }
        
        
    // get the values of the implicit function at the summits
    // [val_0 ... val_7]
    vals[0] = 1.0;//fabs(tsdf0) < 1.0f ? 1.0f - fabs(tsdf0) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf0)*fabs(tsdf0));
    vals[1] = 1.0;//fabs(tsdf1) < 1.0f ? 1.0f - fabs(tsdf1) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf1)*fabs(tsdf1));
    vals[2] = 1.0;//fabs(tsdf2) < 1.0f ? 1.0f - fabs(tsdf2) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf2)*fabs(tsdf2));
    vals[3] = 1.0;//fabs(tsdf3) < 1.0f ? 1.0f - fabs(tsdf3) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf3)*fabs(tsdf3));
    vals[4] = 1.0;//fabs(tsdf4) < 1.0f ? 1.0f - fabs(tsdf4) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf4)*fabs(tsdf4));
    vals[5] = 1.0;//fabs(tsdf5) < 1.0f ? 1.0f - fabs(tsdf5) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf5)*fabs(tsdf5));
    vals[6] = 1.0;//fabs(tsdf6) < 1.0f ? 1.0f - fabs(tsdf6) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf6)*fabs(tsdf6));
    vals[7] = 1.0;//fabs(tsdf7) < 1.0f ? 1.0f - fabs(tsdf7) : 0.00001f; //1.0f/(0.00001f + fabs(tsdf7)*fabs(tsdf7));
    
        
    int nb_faces = ConfigCount[index];
    int offset = Offset[id];
    
    //Compute the position on the edge
    v[0][0] = (vals[0]*s[0][0] + vals[1]*s[1][0])/(vals[0]+vals[1]); v[0][1] = (vals[0]*s[0][1] + vals[1]*s[1][1])/(vals[0]+vals[1]);
    v[1][0] = (vals[1]*s[1][0] + vals[2]*s[2][0])/(vals[1]+vals[2]); v[1][1] = (vals[1]*s[1][1] + vals[2]*s[2][1])/(vals[1]+vals[2]);
    v[2][0] = (vals[2]*s[2][0] + vals[3]*s[3][0])/(vals[2]+vals[3]); v[2][1] = (vals[2]*s[2][1] + vals[3]*s[3][1])/(vals[2]+vals[3]);
    v[3][0] = (vals[0]*s[0][0] + vals[3]*s[3][0])/(vals[0]+vals[3]); v[3][1] = (vals[0]*s[0][1] + vals[3]*s[3][1])/(vals[0]+vals[3]);
    v[4][0] = (vals[1]*s[1][0] + vals[5]*s[5][0])/(vals[1]+vals[5]); v[4][1] = (vals[1]*s[1][1] + vals[5]*s[5][1])/(vals[1]+vals[5]);
    v[5][0] = (vals[2]*s[2][0] + vals[6]*s[6][0])/(vals[2]+vals[6]); v[5][1] = (vals[2]*s[2][1] + vals[6]*s[6][1])/(vals[2]+vals[6]);
    v[6][0] = (vals[3]*s[3][0] + vals[7]*s[7][0])/(vals[3]+vals[7]); v[6][1] = (vals[3]*s[3][1] + vals[7]*s[7][1])/(vals[3]+vals[7]);
    v[7][0] = (vals[0]*s[0][0] + vals[4]*s[4][0])/(vals[0]+vals[4]); v[7][1] = (vals[0]*s[0][1] + vals[4]*s[4][1])/(vals[0]+vals[4]);
    v[8][0] = (vals[4]*s[4][0] + vals[5]*s[5][0])/(vals[4]+vals[5]); v[8][1] = (vals[4]*s[4][1] + vals[5]*s[5][1])/(vals[4]+vals[5]);
    v[9][0] = (vals[5]*s[5][0] + vals[6]*s[6][0])/(vals[5]+vals[6]); v[9][1] = (vals[5]*s[5][1] + vals[6]*s[6][1])/(vals[5]+vals[6]);
    v[10][0] = (vals[6]*s[6][0] + vals[7]*s[7][0])/(vals[6]+vals[7]); v[10][1] = (vals[6]*s[6][1] + vals[7]*s[7][1])/(vals[6]+vals[7]);
    v[11][0] = (vals[4]*s[4][0] + vals[7]*s[7][0])/(vals[4]+vals[7]); v[11][1] = (vals[4]*s[4][1] + vals[7]*s[7][1])/(vals[4]+vals[7]);
    
    v[0][2] = (vals[0]*s[0][2] + vals[1]*s[1][2])/(vals[0]+vals[1]);
    v[1][2] = (vals[1]*s[1][2] + vals[2]*s[2][2])/(vals[1]+vals[2]);
    v[2][2] = (vals[2]*s[2][2] + vals[3]*s[3][2])/(vals[2]+vals[3]);
    v[3][2] = (vals[0]*s[0][2] + vals[3]*s[3][2])/(vals[0]+vals[3]);
    v[4][2] = (vals[1]*s[1][2] + vals[5]*s[5][2])/(vals[1]+vals[5]);
    v[5][2] = (vals[2]*s[2][2] + vals[6]*s[6][2])/(vals[2]+vals[6]);
    v[6][2] = (vals[3]*s[3][2] + vals[7]*s[7][2])/(vals[3]+vals[7]);
    v[7][2] = (vals[0]*s[0][2] + vals[4]*s[4][2])/(vals[0]+vals[4]);
    v[8][2] = (vals[4]*s[4][2] + vals[5]*s[5][2])/(vals[4]+vals[5]);
    v[9][2] = (vals[5]*s[5][2] + vals[6]*s[6][2])/(vals[5]+vals[6]);
    v[10][2] = (vals[6]*s[6][2] + vals[7]*s[7][2])/(vals[6]+vals[7]);
    v[11][2] = (vals[4]*s[4][2] + vals[7]*s[7][2])/(vals[4]+vals[7]);
    
    //Compute the normals
    n[0][0] = (vals[0]*nmle[0][0] + vals[1]*nmle[1][0])/(vals[0]+vals[1]);
    n[0][1] = (vals[0]*nmle[0][1] + vals[1]*nmle[1][1])/(vals[0]+vals[1]);
    n[1][0] = (vals[1]*nmle[1][0] + vals[2]*nmle[2][0])/(vals[1]+vals[2]);
    n[1][1] = (vals[1]*nmle[1][1] + vals[2]*nmle[2][1])/(vals[1]+vals[2]);
    n[2][0] = (vals[2]*nmle[2][0] + vals[3]*nmle[3][0])/(vals[2]+vals[3]);
    n[2][1] = (vals[2]*nmle[2][1] + vals[3]*nmle[3][1])/(vals[2]+vals[3]);
    n[3][0] = (vals[0]*nmle[0][0] + vals[3]*nmle[3][0])/(vals[0]+vals[3]);
    n[3][1] = (vals[0]*nmle[0][1] + vals[3]*nmle[3][1])/(vals[0]+vals[3]);
    n[4][0] = (vals[1]*nmle[1][0] + vals[5]*nmle[5][0])/(vals[1]+vals[5]);
    n[4][1] = (vals[1]*nmle[1][1] + vals[5]*nmle[5][1])/(vals[1]+vals[5]);
    n[5][0] = (vals[2]*nmle[2][0] + vals[6]*nmle[6][0])/(vals[2]+vals[6]);
    n[5][1] = (vals[2]*nmle[2][1] + vals[6]*nmle[6][1])/(vals[2]+vals[6]);
    n[6][0] = (vals[3]*nmle[3][0] + vals[7]*nmle[7][0])/(vals[3]+vals[7]);
    n[6][1] = (vals[3]*nmle[3][1] + vals[7]*nmle[7][1])/(vals[3]+vals[7]);
    n[7][0] = (vals[0]*nmle[0][0] + vals[4]*nmle[4][0])/(vals[0]+vals[4]);
    n[7][1] = (vals[0]*nmle[0][1] + vals[4]*nmle[4][1])/(vals[0]+vals[4]);
    n[8][0] = (vals[4]*nmle[4][0] + vals[5]*nmle[5][0])/(vals[4]+vals[5]);
    n[8][1] = (vals[4]*nmle[4][1] + vals[5]*nmle[5][1])/(vals[4]+vals[5]);
    n[9][0] = (vals[5]*nmle[5][0] + vals[6]*nmle[6][0])/(vals[5]+vals[6]);
    n[9][1] = (vals[5]*nmle[5][1] + vals[6]*nmle[6][1])/(vals[5]+vals[6]);
    n[10][0] = (vals[6]*nmle[6][0] + vals[7]*nmle[7][0])/(vals[6]+vals[7]);
    n[10][1] = (vals[6]*nmle[6][1] + vals[7]*nmle[7][1])/(vals[6]+vals[7]);
    n[11][0] = (vals[4]*nmle[4][0] + vals[7]*nmle[7][0])/(vals[4]+vals[7]);
    n[11][1] = (vals[4]*nmle[4][1] + vals[7]*nmle[7][1])/(vals[4]+vals[7]);
    
    n[0][2] = (vals[0]*nmle[0][2] + vals[1]*nmle[1][2])/(vals[0]+vals[1]);
    n[1][2] = (vals[1]*nmle[1][2] + vals[2]*nmle[2][2])/(vals[1]+vals[2]);
    n[2][2] = (vals[2]*nmle[2][2] + vals[3]*nmle[3][2])/(vals[2]+vals[3]);
    n[3][2] = (vals[0]*nmle[0][2] + vals[3]*nmle[3][2])/(vals[0]+vals[3]);
    n[4][2] = (vals[1]*nmle[1][2] + vals[5]*nmle[5][2])/(vals[1]+vals[5]);
    n[5][2] = (vals[2]*nmle[2][2] + vals[6]*nmle[6][2])/(vals[2]+vals[6]);
    n[6][2] = (vals[3]*nmle[3][2] + vals[7]*nmle[7][2])/(vals[3]+vals[7]);
    n[7][2] = (vals[0]*nmle[0][2] + vals[4]*nmle[4][2])/(vals[0]+vals[4]);
    n[8][2] = (vals[4]*nmle[4][2] + vals[5]*nmle[5][2])/(vals[4]+vals[5]);
    n[9][2] = (vals[5]*nmle[5][2] + vals[6]*nmle[6][2])/(vals[5]+vals[6]);
    n[10][2] = (vals[6]*nmle[6][2] + vals[7]*nmle[7][2])/(vals[6]+vals[7]);
    n[11][2] = (vals[4]*nmle[4][2] + vals[7]*nmle[7][2])/(vals[4]+vals[7]);
    
    for (int i = 0; i < 12; i++) {
        mag = sqrt(n[i][0]*n[i][0] + n[i][1]*n[i][1] + n[i][2]*n[i][2]);
        if (mag > 0.0f) {
            n[i][0] = n[i][0]/mag; n[i][1] = n[i][1]/mag; n[i][2] = n[i][2]/mag;
        }
    }
    
    // add new faces in the list
    int f = 0;
    for ( f = 0; f < nb_faces; f++) {
        if (reverse) {
            Faces[3*(offset+f)] = 3*(offset+f)+2;
            Faces[3*(offset+f) +1] = 3*(offset+f)+1;
            Faces[3*(offset+f) + 2] = 3*(offset+f);
        } else {
            Faces[3*(offset+f)] = 3*(offset+f);
            Faces[3*(offset+f) +1] = 3*(offset+f)+1;
            Faces[3*(offset+f) + 2] = 3*(offset+f)+2;
        }
        
        int id_f_0 = Config[index][f][0];
        int id_f_1 = Config[index][f][1];
        int id_f_2 = Config[index][f][2];
        
        Vertices[9*(offset+f)] = v[id_f_0][0];
        Vertices[9*(offset+f)+1] = v[id_f_0][1];
        Vertices[9*(offset+f)+2] = v[id_f_0][2];
        
        Vertices[9*(offset+f)+3] = v[id_f_1][0];
        Vertices[9*(offset+f)+4] = v[id_f_1][1];
        Vertices[9*(offset+f)+5] = v[id_f_1][2];
        
        Vertices[9*(offset+f)+6] = v[id_f_2][0];
        Vertices[9*(offset+f)+7] = v[id_f_2][1];
        Vertices[9*(offset+f)+8] = v[id_f_2][2];
        
        Normales[9*(offset+f)] = n[id_f_0][0];
        Normales[9*(offset+f)+1] = n[id_f_0][1];
        Normales[9*(offset+f)+2] = n[id_f_0][2];
        
        Normales[9*(offset+f)+3] = n[id_f_1][0];
        Normales[9*(offset+f)+4] = n[id_f_1][1];
        Normales[9*(offset+f)+5] = n[id_f_1][2];
        
        Normales[9*(offset+f)+6] = n[id_f_2][0];
        Normales[9*(offset+f)+7] = n[id_f_2][1];
        Normales[9*(offset+f)+8] = n[id_f_2][2];
        
    }
}


kernel void MarchingCubesIndexing(    constant float2 *TSDF [[ buffer(0) ]],
                                      device int *Offset [[buffer(1)]],
                                      device int *IndexVal [[buffer(2)]],
                                      constant int *Dim [[ buffer(3) ]],
                                      constant float& iso [[ buffer(4) ]],
                                      device atomic_int& faces_counter [[buffer(5)]],
                                  uint2 threadgroup_position_in_grid   [[ threadgroup_position_in_grid ]],
                                  uint2 thread_position_in_threadgroup [[ thread_position_in_threadgroup ]],
                                  uint2 threads_per_threadgroup        [[ threads_per_threadgroup ]]) {
    
    int h = threadgroup_position_in_grid.x * (threads_per_threadgroup.x * threads_per_threadgroup.y) + thread_position_in_threadgroup.x*threads_per_threadgroup.y + thread_position_in_threadgroup.y;
    int z = threadgroup_position_in_grid.y;
    
    int x = (int)(h/Dim[1]);
    int y = h - x*Dim[1];
    
    if (x >= Dim[0]-1 || y >= Dim[1]-1 || z >= Dim[2]-1)
        return;
    
    int id = h*Dim[2] + z;
    
    int s[8][3] = {{x, y, z}, {x+1,y , z}, {x+1, y+1, z}, {x, y + 1, z},
        {x, y, z+1}, {x+1, y, z+1}, {x+1, y+1, z+1}, {x, y+1, z+1}};
    float vals[8] = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    
    // get the values of the implicit function at the summits
    // [val_0 ... val_7]
    for (int k=0; k < 8; k++) {
        vals[k] = TSDF[s[k][2] + Dim[2]*s[k][1] + Dim[2]*Dim[1]*s[k][0]].x;
        if (fabs(vals[k]) >= 1.0f) {
            IndexVal[id] = 0;
            return;
        }
    }
    
    int index;
    // get the index value corresponding to the implicit function
    if (vals[7] <= iso) {
        index = (int)(vals[0] > iso) +
        (int)(vals[1] > iso)*2 +
        (int)(vals[2] > iso)*4 +
        (int)(vals[3] > iso)*8 +
        (int)(vals[4] > iso)*16 +
        (int)(vals[5] > iso)*32 +
        (int)(vals[6] > iso)*64;
        IndexVal[id] = index;
    } else{
        index = (int)(vals[0] <= iso) +
        (int)(vals[1] <= iso)*2 +
        (int)(vals[2] <= iso)*4 +
        (int)(vals[3] <= iso)*8 +
        (int)(vals[4] <= iso)*16 +
        (int)(vals[5] <= iso)*32 +
        (int)(vals[6] <= iso)*64;
        IndexVal[id] = -index;
    }
    
    // get the corresponding configuration
    if (index == 0)
        return;
    
    Offset[id] = atomic_fetch_add_explicit(&faces_counter, ConfigCount[index], memory_order_relaxed);
    
}


kernel void InitArray(    device atomic_int *Array_x [[ buffer(0) ]],
                      device atomic_int *Array_y [[ buffer(1) ]],
                      device atomic_int *Array_z [[ buffer(2) ]],
                      device atomic_int *Weights [[ buffer(3) ]],
                      device atomic_int *Normale_x [[ buffer(4) ]],
                      device atomic_int *Normale_y [[ buffer(5) ]],
                      device atomic_int *Normale_z [[ buffer(6) ]],
                      device float *Vertices [[ buffer(7) ]],
                      constant float *Param [[ buffer(8) ]],
                      constant int *Dim [[ buffer(9) ]],
                      constant int& nb_faces [[ buffer(10) ]],
                      uint2 gid [[ thread_position_in_grid ]],
                      uint2 TgPerGrig [[threadgroups_per_grid]],
                      uint2 tPerTg [[ threads_per_threadgroup ]]) {
    
    int x = gid.x;
    int y = gid.y;
    
    int work_size = TgPerGrig.y*tPerTg.y;
    
    int face_indx = x*work_size + y;
    
    if (face_indx > nb_faces-1)
        return;
    
    float4 pt;
    int indx;
    int coord_i, coord_j, coord_k;
    for (int k = 0; k < 3; k++) {
        pt.x = Vertices[9*face_indx+3*k ];
        pt.y = Vertices[9*face_indx+3*k+1];
        pt.z = Vertices[9*face_indx+3*k+2];
        
        coord_i = max(0, min(Dim[0]-1,(int)(round(pt.x*Param[1]+Param[0]))));
        coord_j = max(0, min(Dim[1]-1,(int)(round(pt.y*Param[3]+Param[2]))));
        coord_k = max(0, min(Dim[2]-1,(int)(round(pt.z*Param[5]+Param[4]))));
        
        indx = coord_i*Dim[2]*Dim[1] + coord_j*Dim[2] + coord_k;
        
        atomic_exchange_explicit(&Array_x[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Array_y[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Array_z[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Normale_x[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Normale_y[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Normale_z[indx], 0, memory_order_relaxed);
        atomic_exchange_explicit(&Weights[indx], 0, memory_order_relaxed);
    }
    
}

kernel void MergeVtx(    device atomic_int *Array_x [[ buffer(0) ]],
                      device atomic_int *Array_y [[ buffer(1) ]],
                      device atomic_int *Array_z [[ buffer(2) ]],
                      device atomic_int *Weights [[ buffer(3) ]],
                      device atomic_int *Normale_x [[ buffer(4) ]],
                      device atomic_int *Normale_y [[ buffer(5) ]],
                      device atomic_int *Normale_z [[ buffer(6) ]],
                      device atomic_int *VtxInd [[ buffer(7) ]],
                      device float *Vertices [[ buffer(8) ]],
                      device int *Faces [[ buffer(9) ]],
                      constant float *Param [[ buffer(10) ]],
                      constant int *Dim [[ buffer(11) ]],
                      device atomic_int& vertex_counter [[ buffer(12) ]],
                      constant int& nb_faces [[ buffer(13) ]],
                      uint2 gid [[ thread_position_in_grid ]],
                     uint2 TgPerGrig [[threadgroups_per_grid]],
                     uint2 tPerTg [[ threads_per_threadgroup ]]) {
    
    int x = gid.x;
    int y = gid.y;
    int work_size = TgPerGrig.y*tPerTg.y;
    
    int face_indx = x*work_size + y;
    
    if (face_indx > nb_faces-1)
        return;
    
    float4 pt[3];
    int indx[3];
    int flag;
    int counter;
    int coord_i, coord_j, coord_k;
    for (int k = 0; k < 3; k++) {
        pt[k].x = Vertices[3*Faces[3*face_indx+k]];
        pt[k].y = Vertices[3*Faces[3*face_indx+k]+1];
        pt[k].z = Vertices[3*Faces[3*face_indx+k]+2];
        
        coord_i = max(0, min(Dim[0]-1,(int)(round(pt[k].x*Param[1]+Param[0]))));
        coord_j = max(0, min(Dim[1]-1,(int)(round(pt[k].y*Param[3]+Param[2]))));
        coord_k = max(0, min(Dim[2]-1,(int)(round(pt[k].z*Param[5]+Param[4]))));
        
        indx[k] = coord_i*Dim[2]*Dim[1] + coord_j*Dim[2] + coord_k;
        
        atomic_fetch_add_explicit(&Array_x[indx[k]], (int)(round(pt[k].x*100000.0f)), memory_order_relaxed);
        atomic_fetch_add_explicit(&Array_y[indx[k]], (int)(round(pt[k].y*100000.0f)), memory_order_relaxed);
        atomic_fetch_add_explicit(&Array_z[indx[k]], (int)(round(pt[k].z*100000.0f)), memory_order_relaxed);
        flag = atomic_fetch_add_explicit(&Weights[indx[k]], 1, memory_order_relaxed);
        Faces[3*face_indx+k] = indx[k];
        
        if (flag == 0) {
            counter = atomic_fetch_add_explicit(&vertex_counter, 1, memory_order_relaxed);
            atomic_exchange_explicit(&VtxInd[indx[k]], counter, memory_order_relaxed);
        }
    }
    
    float4 v1 = (float4)(pt[1].x-pt[0].x, pt[1].y-pt[0].y, pt[1].z-pt[0].z, 1.0f);
    float4 v2 = (float4)(pt[2].x-pt[0].x, pt[2].y-pt[0].y, pt[2].z-pt[0].z, 1.0f);
    float4 nmle = (float4)(v1.y*v2.z - v1.z*v2.y,
                           -v1.x*v2.z + v1.z*v2.x,
                           v1.x*v2.y - v1.y*v2.x, 1.0f);
    for (int k = 0; k < 3; k++) {
        atomic_fetch_add_explicit(&Normale_x[indx[k]], (int)(round(nmle.x*100000.0f)), memory_order_relaxed);
        atomic_fetch_add_explicit(&Normale_y[indx[k]], (int)(round(nmle.y*100000.0f)), memory_order_relaxed);
        atomic_fetch_add_explicit(&Normale_z[indx[k]], (int)(round(nmle.z*100000.0f)), memory_order_relaxed);
    }
    
    
    
}

kernel void SimplifyMesh(    device int *Array_x [[ buffer(0) ]],
                     device int *Array_y [[ buffer(1) ]],
                     device int *Array_z [[ buffer(2) ]],
                     device int *Weights [[ buffer(3) ]],
                     device int *Normale_x [[ buffer(4) ]],
                     device int *Normale_y [[ buffer(5) ]],
                     device int *Normale_z [[ buffer(6) ]],
                     device int *VtxInd [[ buffer(7) ]],
                     device float *Vertices [[ buffer(8) ]],
                     device float *Normales [[ buffer(9) ]],
                     device int *Faces [[ buffer(10) ]],
                     constant int *Dim [[ buffer(11) ]],
                     constant int& nb_faces [[ buffer(12) ]],
                     uint2 gid [[ thread_position_in_grid ]],
                     uint2 TgPerGrig [[threadgroups_per_grid]],
                     uint2 tPerTg [[ threads_per_threadgroup ]] ) {
    
    int x = gid.x;
    int y = gid.y;
    int work_size = TgPerGrig.y*tPerTg.y;
    
    int face_indx = x*work_size + y;
    
    if (face_indx > nb_faces-1)
        return;
    
    int id;
    int id_v;
    float mag;
    for (int k = 0; k < 3; k++) {
        id = Faces[3*face_indx+k];
        id_v = VtxInd[id];
        
        Vertices[3*id_v] = ((float)(Array_x[id])/100000.0f) / (float)(Weights[id]);
        Vertices[3*id_v+1] = ((float)(Array_y[id])/100000.0f) / (float)(Weights[id]);
        Vertices[3*id_v+2] = ((float)(Array_z[id])/100000.0f) / (float)(Weights[id]);
        
        mag = sqrt( (((float)(Normale_x[id]))/100000.0f)*(((float)(Normale_x[id]))/100000.0f) +
                   (((float)(Normale_y[id]))/100000.0f)*(((float)(Normale_y[id]))/100000.0f) +
                   (((float)(Normale_z[id]))/100000.0f)*(((float)(Normale_z[id]))/100000.0f));
        
        if (mag == 0.0f)
            mag = 1.0f;
        Normales[3*id_v] = (((float)(Normale_x[id]))/100000.0f) / mag;
        Normales[3*id_v+1] = (((float)(Normale_y[id]))/100000.0f) / mag;
        Normales[3*id_v+2] = (((float)(Normale_z[id]))/100000.0f) / mag;
        
        
        Faces[3*face_indx + k] = id_v;
    }
    
}
