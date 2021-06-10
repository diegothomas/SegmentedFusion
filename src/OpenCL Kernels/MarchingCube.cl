//    create the precalculated 256 possible polygon configuration (128 + symmetries)


__constant int Config[128][4][3] = { {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}, //0
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

__constant int ConfigCount[128] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 1, 2, 2, 3, 
2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2, 3, 4, 4, 3, 3, 4, 4, 3, 4, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 
3, 2, 3, 3, 4, 3, 4, 4, 3, 3, 4, 4, 3, 4, 3, 3, 2, 2, 3, 3, 4, 3, 4, 2, 3, 3, 4, 4, 3, 4, 3, 3, 2, 3, 4, 4, 3, 4, 3, 3, 2, 4, 3, 
3, 2, 3, 2, 2, 1 };


__kernel void MarchingCubes(__global float *TSDF, __global int *Offset, __global int *IndexVal, 
	__global float * Vertices, __global int *Faces,  __constant float *Param, __constant int *Dim) 
{
        int x = get_global_id(0); /*height*/
        int y = get_global_id(1); /*width*/
		if (Dim[0] <= x + 1 || Dim[1] <= y + 1)return;

        float s[8][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
             {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}};
        
        float v[12][3] = {{0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f}, {0.0f,0.0f, 0.0f},
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
            
        int index;
        int id;
        bool reverse = false;
        int max_z = Dim[2]-1;
        int z = 0;
        for (z = 0; z < max_z; z++) { /*depth*/
			id = z + Dim[0] * y + Dim[0] * Dim[1] * x;
			//id = x + Dim[0]*y + Dim[0]*Dim[1]*z;
            
            // get the index value corresponding to the implicit function
            index = IndexVal[id];
            
            if (index == 0)
                continue;
                
            reverse = false;
            if (index < 0) {
                reverse = true;
                index = -index;
            }
            
            //convert TSDF to float
			//float convVal = 32767.0f;
			float convVal = 0.05f;

            float tsdf0 = (float)(TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*x])/convVal;
            float tsdf1 = (float)(TSDF[z + Dim[0]*y + Dim[0]*Dim[1]*(x+1)])/convVal;
            float tsdf2 = (float)(TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)])/convVal;
            float tsdf3 = (float)(TSDF[z + Dim[0]*(y+1) + Dim[0]*Dim[1]*x])/convVal;
            float tsdf4 = (float)(TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*x])/convVal;
            float tsdf5 = (float)(TSDF[z+1 + Dim[0]*y + Dim[0]*Dim[1]*(x+1)])/convVal;
            float tsdf6 = (float)(TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*(x+1)])/convVal;
            float tsdf7 = (float)(TSDF[z+1 + Dim[0]*(y+1) + Dim[0]*Dim[1]*x])/convVal;


			//float tsdf0 = (float)(TSDF[x + Dim[0] * y + Dim[0] * Dim[1] * z]) / convVal;
			//float tsdf1 = (float)(TSDF[x + Dim[0] * y + Dim[0] * Dim[1] * (z + 1)]) / convVal;
			//float tsdf2 = (float)(TSDF[x + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (z + 1)]) / convVal;
			//float tsdf3 = (float)(TSDF[x + Dim[0] * (y + 1) + Dim[0] * Dim[1] * z]) / convVal;
			//float tsdf4 = (float)(TSDF[x + 1 + Dim[0] * y + Dim[0] * Dim[1] * z]) / convVal;
			//float tsdf5 = (float)(TSDF[x + 1 + Dim[0] * y + Dim[0] * Dim[1] * (z + 1)]) / convVal;
			//float tsdf6 = (float)(TSDF[x + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (z + 1)]) / convVal;
			//float tsdf7 = (float)(TSDF[x + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * z]) / convVal;
            
            // get the values of the implicit function at the summits
            // [val_0 ... val_7]
            vals[0] = 1.0f/(0.00001f + fabs(tsdf0));
            vals[1] = 1.0f/(0.00001f + fabs(tsdf1));
            vals[2] = 1.0f/(0.00001f + fabs(tsdf2));
            vals[3] = 1.0f/(0.00001f + fabs(tsdf3));
            vals[4] = 1.0f/(0.00001f + fabs(tsdf4));
            vals[5] = 1.0f/(0.00001f + fabs(tsdf5));
            vals[6] = 1.0f/(0.00001f + fabs(tsdf6));
            vals[7] = 1.0f/(0.00001f + fabs(tsdf7));
        
            // get the 8  current summits
            s[0][2] = ((float)(z) - Param[4])/Param[5];
            s[1][2] = ((float)(z) - Param[4])/Param[5]; 
            s[2][2] = ((float)(z) - Param[4])/Param[5]; 
            s[3][2] = ((float)(z) - Param[4])/Param[5];
            s[4][2] = ((float)(z+1) - Param[4])/Param[5];
            s[5][2] = ((float)(z+1) - Param[4])/Param[5];
            s[6][2] = ((float)(z+1) - Param[4])/Param[5];
            s[7][2] = ((float)(z+1) - Param[4])/Param[5];
                
            int nb_faces = ConfigCount[index];
            int offset = Offset[id];
            
            //Compute the position on the edge
            v[0][0] = (vals[0]*s[0][0] + vals[1]*s[1][0])/(vals[0]+vals[1]); 
			v[0][1] = (vals[0]*s[0][1] + vals[1]*s[1][1])/(vals[0]+vals[1]);
            v[1][0] = (vals[1]*s[1][0] + vals[2]*s[2][0])/(vals[1]+vals[2]);
			v[1][1] = (vals[1]*s[1][1] + vals[2]*s[2][1])/(vals[1]+vals[2]);
            v[2][0] = (vals[2]*s[2][0] + vals[3]*s[3][0])/(vals[2]+vals[3]);
			v[2][1] = (vals[2]*s[2][1] + vals[3]*s[3][1])/(vals[2]+vals[3]); 
            v[3][0] = (vals[0]*s[0][0] + vals[3]*s[3][0])/(vals[0]+vals[3]); 
			v[3][1] = (vals[0]*s[0][1] + vals[3]*s[3][1])/(vals[0]+vals[3]); 
            v[4][0] = (vals[1]*s[1][0] + vals[5]*s[5][0])/(vals[1]+vals[5]); 
			v[4][1] = (vals[1]*s[1][1] + vals[5]*s[5][1])/(vals[1]+vals[5]); 
            v[5][0] = (vals[2]*s[2][0] + vals[6]*s[6][0])/(vals[2]+vals[6]); 
			v[5][1] = (vals[2]*s[2][1] + vals[6]*s[6][1])/(vals[2]+vals[6]); 
            v[6][0] = (vals[3]*s[3][0] + vals[7]*s[7][0])/(vals[3]+vals[7]); 
			v[6][1] = (vals[3]*s[3][1] + vals[7]*s[7][1])/(vals[3]+vals[7]); 
            v[7][0] = (vals[0]*s[0][0] + vals[4]*s[4][0])/(vals[0]+vals[4]); 
			v[7][1] = (vals[0]*s[0][1] + vals[4]*s[4][1])/(vals[0]+vals[4]); 
            v[8][0] = (vals[4]*s[4][0] + vals[5]*s[5][0])/(vals[4]+vals[5]); 
			v[8][1] = (vals[4]*s[4][1] + vals[5]*s[5][1])/(vals[4]+vals[5]); 
            v[9][0] = (vals[5]*s[5][0] + vals[6]*s[6][0])/(vals[5]+vals[6]); 
			v[9][1] = (vals[5]*s[5][1] + vals[6]*s[6][1])/(vals[5]+vals[6]); 
            v[10][0] = (vals[6]*s[6][0] + vals[7]*s[7][0])/(vals[6]+vals[7]);
			v[10][1] = (vals[6]*s[6][1] + vals[7]*s[7][1])/(vals[6]+vals[7]);
            v[11][0] = (vals[4]*s[4][0] + vals[7]*s[7][0])/(vals[4]+vals[7]);
			v[11][1] = (vals[4]*s[4][1] + vals[7]*s[7][1])/(vals[4]+vals[7]);
            
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
                    
                    Vertices[9*(offset+f)] = v[Config[index][f][0]][0];
                    Vertices[9*(offset+f)+1] = v[Config[index][f][0]][1];
                    Vertices[9*(offset+f)+2] = v[Config[index][f][0]][2];
                    
                    Vertices[9*(offset+f)+3] = v[Config[index][f][1]][0];
                    Vertices[9*(offset+f)+4] = v[Config[index][f][1]][1];
                    Vertices[9*(offset+f)+5] = v[Config[index][f][1]][2];
                    
                    Vertices[9*(offset+f)+6] = v[Config[index][f][2]][0];
                    Vertices[9*(offset+f)+7] = v[Config[index][f][2]][1];
                    Vertices[9*(offset+f)+8] = v[Config[index][f][2]][2];
                    
                   
            }
               
        }
}

__kernel void MarchingCubes_normal(__global float *TSDF, __global int *Offset, __global int *IndexVal, 
	__global float * VtxNmle, __global int *Faces, __constant float *Param, __constant int *Dim)
{
	int h = get_global_id(0); /*height*/
	int w = get_global_id(1); /*width*/

	int x = (int)(h % Dim[0]);
	int y = (int)(h / Dim[0]);
	int z = w;

	if (x < 1 || x >= Dim[0] - 2 || y < 1 || y >= Dim[1] - 2 || z < 1 || z >= Dim[2] - 2)
		return;

	int id = x * Dim[2] *Dim[1] + y * Dim[2] + z;
	int index = IndexVal[id];
	int stride = Dim[0] * Dim[1] * Dim[2] * 9;

	if (index == 0)
		return;

	float s[8][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };


	float nmle[8][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float v[12][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float n[12][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float vals[8] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

	// get the 8  current summits
	s[0][0] = ((float)(x)-Param[0]) / Param[1];
	s[0][1] = ((float)(y)-Param[2]) / Param[3];
	s[1][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[1][1] = ((float)(y)-Param[2]) / Param[3];
	s[2][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[2][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[3][0] = ((float)(x)-Param[0]) / Param[1];
	s[3][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[4][0] = ((float)(x)-Param[0]) / Param[1];
	s[4][1] = ((float)(y)-Param[2]) / Param[3];
	s[5][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[5][1] = ((float)(y)-Param[2]) / Param[3];
	s[6][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[6][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[7][0] = ((float)(x)-Param[0]) / Param[1];
	s[7][1] = ((float)(y + 1) - Param[2]) / Param[3];

	// get the 8  current summits
	s[0][2] = ((float)(z)-Param[4]) / Param[5];
	s[1][2] = ((float)(z)-Param[4]) / Param[5];
	s[2][2] = ((float)(z)-Param[4]) / Param[5];
	s[3][2] = ((float)(z)-Param[4]) / Param[5];
	s[4][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[5][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[6][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[7][2] = ((float)(z + 1) - Param[4]) / Param[5];

	bool reverse = false;
	if (index < 0) {
		reverse = true;
		index = -index;
	}

	//convert TSDF to float
	float tsdf0 = Param[1] * 0.05f*TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x];
	float tsdf1 = Param[1] * 0.05f*TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	float tsdf2 = Param[1] * 0.05f*TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	float tsdf3 = Param[1] * 0.05f*TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	float tsdf4 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x];
	float tsdf5 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	float tsdf6 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	float tsdf7 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];

	// Compute normals at grid vertices
	nmle[0][0] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	nmle[0][1] = TSDF[z + Dim[0] * (y - 1) + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	nmle[0][2] = TSDF[(z - 1) + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[(z + 1) + Dim[0] * y + Dim[0] * Dim[1] * x];
	float mag = sqrt(nmle[0][0] * nmle[0][0] + nmle[0][1] * nmle[0][1] + nmle[0][2] * nmle[0][2]);
	if (mag > 0.0f) {
		nmle[0][0] = nmle[0][0] / mag; nmle[0][1] = nmle[0][1] / mag; nmle[0][2] = nmle[0][2] / mag;
	}

	nmle[1][0] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 2)];
	nmle[1][1] = TSDF[z + Dim[0] * (y - 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[1][2] = TSDF[(z - 1) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 1) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[1][0] * nmle[1][0] + nmle[1][1] * nmle[1][1] + nmle[1][2] * nmle[1][2]);
	if (mag > 0.0f) {
		nmle[1][0] = nmle[1][0] / mag; nmle[1][1] = nmle[1][1] / mag; nmle[1][2] = nmle[1][2] / mag;
	}

	nmle[2][0] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 2)];
	nmle[2][1] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + Dim[0] * (y + 2) + Dim[0] * Dim[1] * (x + 1)];
	nmle[2][2] = TSDF[(z - 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[2][0] * nmle[2][0] + nmle[2][1] * nmle[2][1] + nmle[2][2] * nmle[2][2]);
	if (mag > 0.0f) {
		nmle[2][0] = nmle[2][0] / mag; nmle[2][1] = nmle[2][1] / mag; nmle[2][2] = nmle[2][2] / mag;
	}

	nmle[3][0] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[3][1] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 2) + Dim[0] * Dim[1] * x];
	nmle[3][2] = TSDF[(z - 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[(z + 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[3][0] * nmle[3][0] + nmle[3][1] * nmle[3][1] + nmle[3][2] * nmle[3][2]);
	if (mag > 0.0f) {
		nmle[3][0] = nmle[3][0] / mag; nmle[3][1] = nmle[3][1] / mag; nmle[3][2] = nmle[3][2] / mag;
	}

	nmle[4][0] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	nmle[4][1] = TSDF[z + 1 + Dim[0] * (y - 1) + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	nmle[4][2] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[(z + 2) + Dim[0] * y + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[4][0] * nmle[4][0] + nmle[4][1] * nmle[4][1] + nmle[4][2] * nmle[4][2]);
	if (mag > 0.0f) {
		nmle[4][0] = nmle[4][0] / mag; nmle[4][1] = nmle[4][1] / mag; nmle[4][2] = nmle[4][2] / mag;
	}

	nmle[5][0] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 2)];
	nmle[5][1] = TSDF[z + 1 + Dim[0] * (y - 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[5][2] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 2) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[5][0] * nmle[5][0] + nmle[5][1] * nmle[5][1] + nmle[5][2] * nmle[5][2]);
	if (mag > 0.0f) {
		nmle[5][0] = nmle[5][0] / mag; nmle[5][1] = nmle[5][1] / mag; nmle[5][2] = nmle[5][2] / mag;
	}

	nmle[6][0] = TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 2)];
	nmle[6][1] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + 1 + Dim[0] * (y + 2) + Dim[0] * Dim[1] * (x + 1)];
	nmle[6][2] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 2) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[6][0] * nmle[6][0] + nmle[6][1] * nmle[6][1] + nmle[6][2] * nmle[6][2]);
	if (mag > 0.0f) {
		nmle[6][0] = nmle[6][0] / mag; nmle[6][1] = nmle[6][1] / mag; nmle[6][2] = nmle[6][2] / mag;
	}

	nmle[7][0] = TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[7][1] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 2) + Dim[0] * Dim[1] * x];
	nmle[7][2] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[(z + 2) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[7][0] * nmle[7][0] + nmle[7][1] * nmle[7][1] + nmle[7][2] * nmle[7][2]);
	if (mag > 0.0f) {
		nmle[7][0] = nmle[7][0] / mag; nmle[7][1] = nmle[7][1] / mag; nmle[7][2] = nmle[7][2] / mag;
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
	v[0][0] = (vals[0] * s[0][0] + vals[1] * s[1][0]) / (vals[0] + vals[1]); v[0][1] = (vals[0] * s[0][1] + vals[1] * s[1][1]) / (vals[0] + vals[1]);
	v[1][0] = (vals[1] * s[1][0] + vals[2] * s[2][0]) / (vals[1] + vals[2]); v[1][1] = (vals[1] * s[1][1] + vals[2] * s[2][1]) / (vals[1] + vals[2]);
	v[2][0] = (vals[2] * s[2][0] + vals[3] * s[3][0]) / (vals[2] + vals[3]); v[2][1] = (vals[2] * s[2][1] + vals[3] * s[3][1]) / (vals[2] + vals[3]);
	v[3][0] = (vals[0] * s[0][0] + vals[3] * s[3][0]) / (vals[0] + vals[3]); v[3][1] = (vals[0] * s[0][1] + vals[3] * s[3][1]) / (vals[0] + vals[3]);
	v[4][0] = (vals[1] * s[1][0] + vals[5] * s[5][0]) / (vals[1] + vals[5]); v[4][1] = (vals[1] * s[1][1] + vals[5] * s[5][1]) / (vals[1] + vals[5]);
	v[5][0] = (vals[2] * s[2][0] + vals[6] * s[6][0]) / (vals[2] + vals[6]); v[5][1] = (vals[2] * s[2][1] + vals[6] * s[6][1]) / (vals[2] + vals[6]);
	v[6][0] = (vals[3] * s[3][0] + vals[7] * s[7][0]) / (vals[3] + vals[7]); v[6][1] = (vals[3] * s[3][1] + vals[7] * s[7][1]) / (vals[3] + vals[7]);
	v[7][0] = (vals[0] * s[0][0] + vals[4] * s[4][0]) / (vals[0] + vals[4]); v[7][1] = (vals[0] * s[0][1] + vals[4] * s[4][1]) / (vals[0] + vals[4]);
	v[8][0] = (vals[4] * s[4][0] + vals[5] * s[5][0]) / (vals[4] + vals[5]); v[8][1] = (vals[4] * s[4][1] + vals[5] * s[5][1]) / (vals[4] + vals[5]);
	v[9][0] = (vals[5] * s[5][0] + vals[6] * s[6][0]) / (vals[5] + vals[6]); v[9][1] = (vals[5] * s[5][1] + vals[6] * s[6][1]) / (vals[5] + vals[6]);
	v[10][0] = (vals[6] * s[6][0] + vals[7] * s[7][0]) / (vals[6] + vals[7]); v[10][1] = (vals[6] * s[6][1] + vals[7] * s[7][1]) / (vals[6] + vals[7]);
	v[11][0] = (vals[4] * s[4][0] + vals[7] * s[7][0]) / (vals[4] + vals[7]); v[11][1] = (vals[4] * s[4][1] + vals[7] * s[7][1]) / (vals[4] + vals[7]);

	v[0][2] = (vals[0] * s[0][2] + vals[1] * s[1][2]) / (vals[0] + vals[1]);
	v[1][2] = (vals[1] * s[1][2] + vals[2] * s[2][2]) / (vals[1] + vals[2]);
	v[2][2] = (vals[2] * s[2][2] + vals[3] * s[3][2]) / (vals[2] + vals[3]);
	v[3][2] = (vals[0] * s[0][2] + vals[3] * s[3][2]) / (vals[0] + vals[3]);
	v[4][2] = (vals[1] * s[1][2] + vals[5] * s[5][2]) / (vals[1] + vals[5]);
	v[5][2] = (vals[2] * s[2][2] + vals[6] * s[6][2]) / (vals[2] + vals[6]);
	v[6][2] = (vals[3] * s[3][2] + vals[7] * s[7][2]) / (vals[3] + vals[7]);
	v[7][2] = (vals[0] * s[0][2] + vals[4] * s[4][2]) / (vals[0] + vals[4]);
	v[8][2] = (vals[4] * s[4][2] + vals[5] * s[5][2]) / (vals[4] + vals[5]);
	v[9][2] = (vals[5] * s[5][2] + vals[6] * s[6][2]) / (vals[5] + vals[6]);
	v[10][2] = (vals[6] * s[6][2] + vals[7] * s[7][2]) / (vals[6] + vals[7]);
	v[11][2] = (vals[4] * s[4][2] + vals[7] * s[7][2]) / (vals[4] + vals[7]);

	//Compute the normals
	n[0][0] = (vals[0] * nmle[0][0] + vals[1] * nmle[1][0]) / (vals[0] + vals[1]);
	n[0][1] = (vals[0] * nmle[0][1] + vals[1] * nmle[1][1]) / (vals[0] + vals[1]);
	n[1][0] = (vals[1] * nmle[1][0] + vals[2] * nmle[2][0]) / (vals[1] + vals[2]);
	n[1][1] = (vals[1] * nmle[1][1] + vals[2] * nmle[2][1]) / (vals[1] + vals[2]);
	n[2][0] = (vals[2] * nmle[2][0] + vals[3] * nmle[3][0]) / (vals[2] + vals[3]);
	n[2][1] = (vals[2] * nmle[2][1] + vals[3] * nmle[3][1]) / (vals[2] + vals[3]);
	n[3][0] = (vals[0] * nmle[0][0] + vals[3] * nmle[3][0]) / (vals[0] + vals[3]);
	n[3][1] = (vals[0] * nmle[0][1] + vals[3] * nmle[3][1]) / (vals[0] + vals[3]);
	n[4][0] = (vals[1] * nmle[1][0] + vals[5] * nmle[5][0]) / (vals[1] + vals[5]);
	n[4][1] = (vals[1] * nmle[1][1] + vals[5] * nmle[5][1]) / (vals[1] + vals[5]);
	n[5][0] = (vals[2] * nmle[2][0] + vals[6] * nmle[6][0]) / (vals[2] + vals[6]);
	n[5][1] = (vals[2] * nmle[2][1] + vals[6] * nmle[6][1]) / (vals[2] + vals[6]);
	n[6][0] = (vals[3] * nmle[3][0] + vals[7] * nmle[7][0]) / (vals[3] + vals[7]);
	n[6][1] = (vals[3] * nmle[3][1] + vals[7] * nmle[7][1]) / (vals[3] + vals[7]);
	n[7][0] = (vals[0] * nmle[0][0] + vals[4] * nmle[4][0]) / (vals[0] + vals[4]);
	n[7][1] = (vals[0] * nmle[0][1] + vals[4] * nmle[4][1]) / (vals[0] + vals[4]);
	n[8][0] = (vals[4] * nmle[4][0] + vals[5] * nmle[5][0]) / (vals[4] + vals[5]);
	n[8][1] = (vals[4] * nmle[4][1] + vals[5] * nmle[5][1]) / (vals[4] + vals[5]);
	n[9][0] = (vals[5] * nmle[5][0] + vals[6] * nmle[6][0]) / (vals[5] + vals[6]);
	n[9][1] = (vals[5] * nmle[5][1] + vals[6] * nmle[6][1]) / (vals[5] + vals[6]);
	n[10][0] = (vals[6] * nmle[6][0] + vals[7] * nmle[7][0]) / (vals[6] + vals[7]);
	n[10][1] = (vals[6] * nmle[6][1] + vals[7] * nmle[7][1]) / (vals[6] + vals[7]);
	n[11][0] = (vals[4] * nmle[4][0] + vals[7] * nmle[7][0]) / (vals[4] + vals[7]);
	n[11][1] = (vals[4] * nmle[4][1] + vals[7] * nmle[7][1]) / (vals[4] + vals[7]);

	n[0][2] = (vals[0] * nmle[0][2] + vals[1] * nmle[1][2]) / (vals[0] + vals[1]);
	n[1][2] = (vals[1] * nmle[1][2] + vals[2] * nmle[2][2]) / (vals[1] + vals[2]);
	n[2][2] = (vals[2] * nmle[2][2] + vals[3] * nmle[3][2]) / (vals[2] + vals[3]);
	n[3][2] = (vals[0] * nmle[0][2] + vals[3] * nmle[3][2]) / (vals[0] + vals[3]);
	n[4][2] = (vals[1] * nmle[1][2] + vals[5] * nmle[5][2]) / (vals[1] + vals[5]);
	n[5][2] = (vals[2] * nmle[2][2] + vals[6] * nmle[6][2]) / (vals[2] + vals[6]);
	n[6][2] = (vals[3] * nmle[3][2] + vals[7] * nmle[7][2]) / (vals[3] + vals[7]);
	n[7][2] = (vals[0] * nmle[0][2] + vals[4] * nmle[4][2]) / (vals[0] + vals[4]);
	n[8][2] = (vals[4] * nmle[4][2] + vals[5] * nmle[5][2]) / (vals[4] + vals[5]);
	n[9][2] = (vals[5] * nmle[5][2] + vals[6] * nmle[6][2]) / (vals[5] + vals[6]);
	n[10][2] = (vals[6] * nmle[6][2] + vals[7] * nmle[7][2]) / (vals[6] + vals[7]);
	n[11][2] = (vals[4] * nmle[4][2] + vals[7] * nmle[7][2]) / (vals[4] + vals[7]);

	int i = 0;
	for (i = 0; i < 12; i++) {
		mag = sqrt(n[i][0] * n[i][0] + n[i][1] * n[i][1] + n[i][2] * n[i][2]);
		if (mag > 0.0f) {
			n[i][0] = n[i][0] / mag; n[i][1] = n[i][1] / mag; n[i][2] = n[i][2] / mag;
		}
	}

	// add new faces in the list
	int f = 0;
	for (f = 0; f < nb_faces; f++) {
		if (reverse) {
			Faces[3 * (offset + f)] = 3 * (offset + f) + 2;
			Faces[3 * (offset + f) + 1] = 3 * (offset + f) + 1;
			Faces[3 * (offset + f) + 2] = 3 * (offset + f);
		}
		else {
			Faces[3 * (offset + f)] = 3 * (offset + f);
			Faces[3 * (offset + f) + 1] = 3 * (offset + f) + 1;
			Faces[3 * (offset + f) + 2] = 3 * (offset + f) + 2;
		}

		int id_f_0 = Config[index][f][0];
		int id_f_1 = Config[index][f][1];
		int id_f_2 = Config[index][f][2];

		VtxNmle[9 * (offset + f)] = v[id_f_0][0];
		VtxNmle[9 * (offset + f) + 1] = v[id_f_0][1];
		VtxNmle[9 * (offset + f) + 2] = v[id_f_0][2];

		VtxNmle[9 * (offset + f) + 3] = v[id_f_1][0];
		VtxNmle[9 * (offset + f) + 4] = v[id_f_1][1];
		VtxNmle[9 * (offset + f) + 5] = v[id_f_1][2];

		VtxNmle[9 * (offset + f) + 6] = v[id_f_2][0];
		VtxNmle[9 * (offset + f) + 7] = v[id_f_2][1];
		VtxNmle[9 * (offset + f) + 8] = v[id_f_2][2];

		VtxNmle[9 * (offset + f) + 0 + stride] = -n[id_f_0][0];
		VtxNmle[9 * (offset + f) + 1 + stride] = -n[id_f_0][1];
		VtxNmle[9 * (offset + f) + 2 + stride] = -n[id_f_0][2];

		VtxNmle[9 * (offset + f) + 3 + stride] = -n[id_f_1][0];
		VtxNmle[9 * (offset + f) + 4 + stride] = -n[id_f_1][1];
		VtxNmle[9 * (offset + f) + 5 + stride] = -n[id_f_1][2];

		VtxNmle[9 * (offset + f) + 6 + stride] = -n[id_f_2][0];
		VtxNmle[9 * (offset + f) + 7 + stride] = -n[id_f_2][1];
		VtxNmle[9 * (offset + f) + 8 + stride] = -n[id_f_2][2];

	}
}


__kernel void MarchingCubes_oneFunc(
	__global float *TSDF,//TSDF
	__global int *Faces, //Faces
	__constant float *Param, //[x,cx,y,cy,z,cz]
	__global int *faces_counter,
	const float iso,
	__constant int *Dim, //[x,y,z]
	__global float *VtxNmle
	//,__read_only image2d_t color
)
{
	int h = get_global_id(0); /*height*/
	int w = get_global_id(1); /*width*/

	int x = (int)(h % Dim[0]);
	int y = (int)(h / Dim[0]);
	int z = w;
	//x = h; y = w;
	int stride = Dim[0] * Dim[1] * Dim[2] * 9;
	//return;

	if (x >= Dim[0] - 1 || y >= Dim[1] - 1 || z >= Dim[2] - 1)return;

	float s[8][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	int sId[8][3] = { { x, y, z },{ x + 1,y , z },{ x + 1, y + 1, z },{ x, y + 1, z },
	{ x, y, z + 1 },{ x + 1, y, z + 1 },{ x + 1, y + 1, z + 1 },{ x, y + 1, z + 1 } };

	float v[12][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float n[12][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float nmle[8][3] = { { 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },
	{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f },{ 0.0f,0.0f, 0.0f } };

	float vals[8] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
	float convVal = 0.05;

	// get the 8  current summits
	s[0][0] = ((float)(x)-Param[0]) / Param[1];
	s[0][1] = ((float)(y)-Param[2]) / Param[3];
	s[1][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[1][1] = ((float)(y)-Param[2]) / Param[3];
	s[2][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[2][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[3][0] = ((float)(x)-Param[0]) / Param[1];
	s[3][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[4][0] = ((float)(x)-Param[0]) / Param[1];
	s[4][1] = ((float)(y)-Param[2]) / Param[3];
	s[5][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[5][1] = ((float)(y)-Param[2]) / Param[3];
	s[6][0] = ((float)(x + 1) - Param[0]) / Param[1];
	s[6][1] = ((float)(y + 1) - Param[2]) / Param[3];
	s[7][0] = ((float)(x)-Param[0]) / Param[1];
	s[7][1] = ((float)(y + 1) - Param[2]) / Param[3];

	// get the 8  current summits
	s[0][2] = ((float)(z)-Param[4]) / Param[5];
	s[1][2] = ((float)(z)-Param[4]) / Param[5];
	s[2][2] = ((float)(z)-Param[4]) / Param[5];
	s[3][2] = ((float)(z)-Param[4]) / Param[5];
	s[4][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[5][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[6][2] = ((float)(z + 1) - Param[4]) / Param[5];
	s[7][2] = ((float)(z + 1) - Param[4]) / Param[5];

	for (int k = 0; k < 8; k++) {
		vals[k] = (float)(TSDF[sId[k][0] + Dim[0] * sId[k][1] + Dim[0] * Dim[1] * sId[k][2]]) / convVal;
		vals[k] = (float)(TSDF[sId[k][2] + Dim[2] * sId[k][1] + Dim[2] * Dim[1] * sId[k][0]]) / convVal;
		//vals[k] = TSDF[sId[k][2] + Dim[2] * sId[k][1] + Dim[2] * Dim[1] * sId[k][0]];
		if (fabs(vals[k]) >= 1.0f) {
			return;
		}
	}

	int index;
	bool reverse = false;

	// get the index value corresponding to the implicit function
	if (vals[7] <= iso) {
		index = (int)(vals[0] > iso) +
			(int)(vals[1] > iso) * 2 +
			(int)(vals[2] > iso) * 4 +
			(int)(vals[3] > iso) * 8 +
			(int)(vals[4] > iso) * 16 +
			(int)(vals[5] > iso) * 32 +
			(int)(vals[6] > iso) * 64;
	}
	else {
		index = (int)(vals[0] <= iso) +
			(int)(vals[1] <= iso) * 2 +
			(int)(vals[2] <= iso) * 4 +
			(int)(vals[3] <= iso) * 8 +
			(int)(vals[4] <= iso) * 16 +
			(int)(vals[5] <= iso) * 32 +
			(int)(vals[6] <= iso) * 64;
		reverse = true;
	}

	if (index == 0)return;
	if (index < 0) {
		reverse = true;
		index = -index;
	}

	//convert TSDF to float
	float tsdf0 = Param[1] * 0.05f*TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x];
	float tsdf1 = Param[1] * 0.05f*TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	float tsdf2 = Param[1] * 0.05f*TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	float tsdf3 = Param[1] * 0.05f*TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	float tsdf4 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x];
	float tsdf5 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	float tsdf6 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	float tsdf7 = Param[1] * 0.05f*TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];

	// Compute normals at grid vertices
	nmle[0][0] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	nmle[0][1] = TSDF[z + Dim[0] * (y - 1) + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	nmle[0][2] = TSDF[(z - 1) + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[(z + 1) + Dim[0] * y + Dim[0] * Dim[1] * x];
	float mag = sqrt(nmle[0][0] * nmle[0][0] + nmle[0][1] * nmle[0][1] + nmle[0][2] * nmle[0][2]);
	if (mag > 0.0f) {
		nmle[0][0] = nmle[0][0] / mag; nmle[0][1] = nmle[0][1] / mag; nmle[0][2] = nmle[0][2] / mag;
	}

	nmle[1][0] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 2)];
	nmle[1][1] = TSDF[z + Dim[0] * (y - 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[1][2] = TSDF[(z - 1) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 1) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[1][0] * nmle[1][0] + nmle[1][1] * nmle[1][1] + nmle[1][2] * nmle[1][2]);
	if (mag > 0.0f) {
		nmle[1][0] = nmle[1][0] / mag; nmle[1][1] = nmle[1][1] / mag; nmle[1][2] = nmle[1][2] / mag;
	}

	nmle[2][0] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 2)];
	nmle[2][1] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + Dim[0] * (y + 2) + Dim[0] * Dim[1] * (x + 1)];
	nmle[2][2] = TSDF[(z - 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[2][0] * nmle[2][0] + nmle[2][1] * nmle[2][1] + nmle[2][2] * nmle[2][2]);
	if (mag > 0.0f) {
		nmle[2][0] = nmle[2][0] / mag; nmle[2][1] = nmle[2][1] / mag; nmle[2][2] = nmle[2][2] / mag;
	}

	nmle[3][0] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[3][1] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + Dim[0] * (y + 2) + Dim[0] * Dim[1] * x];
	nmle[3][2] = TSDF[(z - 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[(z + 1) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[3][0] * nmle[3][0] + nmle[3][1] * nmle[3][1] + nmle[3][2] * nmle[3][2]);
	if (mag > 0.0f) {
		nmle[3][0] = nmle[3][0] / mag; nmle[3][1] = nmle[3][1] / mag; nmle[3][2] = nmle[3][2] / mag;
	}

	nmle[4][0] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x- 1)] - TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	nmle[4][1] = TSDF[z + 1 + Dim[0] * (y - 1) + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	nmle[4][2] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[(z + 2) + Dim[0] * y + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[4][0] * nmle[4][0] + nmle[4][1] * nmle[4][1] + nmle[4][2] * nmle[4][2]);
	if (mag > 0.0f) {
		nmle[4][0] = nmle[4][0] / mag; nmle[4][1] = nmle[4][1] / mag; nmle[4][2] = nmle[4][2] / mag;
	}

	nmle[5][0] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 2)];
	nmle[5][1] = TSDF[z + 1 + Dim[0] * (y - 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[5][2] = TSDF[z + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 2) + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[5][0] * nmle[5][0] + nmle[5][1] * nmle[5][1] + nmle[5][2] * nmle[5][2]);
	if (mag > 0.0f) {
		nmle[5][0] = nmle[5][0] / mag; nmle[5][1] = nmle[5][1] / mag; nmle[5][2] = nmle[5][2] / mag;
	}

	nmle[6][0] = TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 2)];
	nmle[6][1] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * (x + 1)] - TSDF[z + 1 + Dim[0] * (y + 2) + Dim[0] * Dim[1] * (x + 1)];
	nmle[6][2] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)] - TSDF[(z + 2) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	mag = sqrt(nmle[6][0] * nmle[6][0] + nmle[6][1] * nmle[6][1] + nmle[6][2] * nmle[6][2]);
	if (mag > 0.0f) {
		nmle[6][0] = nmle[6][0] / mag; nmle[6][1] = nmle[6][1] / mag; nmle[6][2] = nmle[6][2] / mag;
	}

	nmle[7][0] = TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x - 1)] - TSDF[z + 1 + Dim[0] * (y + 1) + Dim[0] * Dim[1] * (x + 1)];
	nmle[7][1] = TSDF[z + 1 + Dim[0] * y + Dim[0] * Dim[1] * x] - TSDF[z + 1 + Dim[0] * (y + 2) + Dim[0] * Dim[1] * x];
	nmle[7][2] = TSDF[z + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x] - TSDF[(z + 2) + Dim[0] * (y + 1) + Dim[0] * Dim[1] * x];
	mag = sqrt(nmle[7][0] * nmle[7][0] + nmle[7][1] * nmle[7][1] + nmle[7][2] * nmle[7][2]);
	if (mag > 0.0f) {
		nmle[7][0] = nmle[7][0] / mag; nmle[7][1] = nmle[7][1] / mag; nmle[7][2] = nmle[7][2] / mag;
	}

	for (int j = 0; j < 8; j++) {
		vals[j] = 1.0f; // 1.0f / (0.00001f + fabs(vals[j]));
	}
	int nb_faces = ConfigCount[index];
	int offset = atomic_add(faces_counter, nb_faces);

	//Compute the position on the edge
	v[0][0] = (vals[0] * s[0][0] + vals[1] * s[1][0]) / (vals[0] + vals[1]); v[0][1] = (vals[0] * s[0][1] + vals[1] * s[1][1]) / (vals[0] + vals[1]);
	v[1][0] = (vals[1] * s[1][0] + vals[2] * s[2][0]) / (vals[1] + vals[2]); v[1][1] = (vals[1] * s[1][1] + vals[2] * s[2][1]) / (vals[1] + vals[2]);
	v[2][0] = (vals[2] * s[2][0] + vals[3] * s[3][0]) / (vals[2] + vals[3]); v[2][1] = (vals[2] * s[2][1] + vals[3] * s[3][1]) / (vals[2] + vals[3]);
	v[3][0] = (vals[0] * s[0][0] + vals[3] * s[3][0]) / (vals[0] + vals[3]); v[3][1] = (vals[0] * s[0][1] + vals[3] * s[3][1]) / (vals[0] + vals[3]);
	v[4][0] = (vals[1] * s[1][0] + vals[5] * s[5][0]) / (vals[1] + vals[5]); v[4][1] = (vals[1] * s[1][1] + vals[5] * s[5][1]) / (vals[1] + vals[5]);
	v[5][0] = (vals[2] * s[2][0] + vals[6] * s[6][0]) / (vals[2] + vals[6]); v[5][1] = (vals[2] * s[2][1] + vals[6] * s[6][1]) / (vals[2] + vals[6]);
	v[6][0] = (vals[3] * s[3][0] + vals[7] * s[7][0]) / (vals[3] + vals[7]); v[6][1] = (vals[3] * s[3][1] + vals[7] * s[7][1]) / (vals[3] + vals[7]);
	v[7][0] = (vals[0] * s[0][0] + vals[4] * s[4][0]) / (vals[0] + vals[4]); v[7][1] = (vals[0] * s[0][1] + vals[4] * s[4][1]) / (vals[0] + vals[4]);
	v[8][0] = (vals[4] * s[4][0] + vals[5] * s[5][0]) / (vals[4] + vals[5]); v[8][1] = (vals[4] * s[4][1] + vals[5] * s[5][1]) / (vals[4] + vals[5]);
	v[9][0] = (vals[5] * s[5][0] + vals[6] * s[6][0]) / (vals[5] + vals[6]); v[9][1] = (vals[5] * s[5][1] + vals[6] * s[6][1]) / (vals[5] + vals[6]);
	v[10][0] = (vals[6] * s[6][0] + vals[7] * s[7][0]) / (vals[6] + vals[7]); v[10][1] = (vals[6] * s[6][1] + vals[7] * s[7][1]) / (vals[6] + vals[7]);
	v[11][0] = (vals[4] * s[4][0] + vals[7] * s[7][0]) / (vals[4] + vals[7]); v[11][1] = (vals[4] * s[4][1] + vals[7] * s[7][1]) / (vals[4] + vals[7]);

	v[0][2] = (vals[0] * s[0][2] + vals[1] * s[1][2]) / (vals[0] + vals[1]);
	v[1][2] = (vals[1] * s[1][2] + vals[2] * s[2][2]) / (vals[1] + vals[2]);
	v[2][2] = (vals[2] * s[2][2] + vals[3] * s[3][2]) / (vals[2] + vals[3]);
	v[3][2] = (vals[0] * s[0][2] + vals[3] * s[3][2]) / (vals[0] + vals[3]);
	v[4][2] = (vals[1] * s[1][2] + vals[5] * s[5][2]) / (vals[1] + vals[5]);
	v[5][2] = (vals[2] * s[2][2] + vals[6] * s[6][2]) / (vals[2] + vals[6]);
	v[6][2] = (vals[3] * s[3][2] + vals[7] * s[7][2]) / (vals[3] + vals[7]);
	v[7][2] = (vals[0] * s[0][2] + vals[4] * s[4][2]) / (vals[0] + vals[4]);
	v[8][2] = (vals[4] * s[4][2] + vals[5] * s[5][2]) / (vals[4] + vals[5]);
	v[9][2] = (vals[5] * s[5][2] + vals[6] * s[6][2]) / (vals[5] + vals[6]);
	v[10][2] = (vals[6] * s[6][2] + vals[7] * s[7][2]) / (vals[6] + vals[7]);
	v[11][2] = (vals[4] * s[4][2] + vals[7] * s[7][2]) / (vals[4] + vals[7]);

	//Compute the normals
	n[0][0] = (vals[0] * nmle[0][0] + vals[1] * nmle[1][0]) / (vals[0] + vals[1]);
	n[0][1] = (vals[0] * nmle[0][1] + vals[1] * nmle[1][1]) / (vals[0] + vals[1]);
	n[1][0] = (vals[1] * nmle[1][0] + vals[2] * nmle[2][0]) / (vals[1] + vals[2]);
	n[1][1] = (vals[1] * nmle[1][1] + vals[2] * nmle[2][1]) / (vals[1] + vals[2]);
	n[2][0] = (vals[2] * nmle[2][0] + vals[3] * nmle[3][0]) / (vals[2] + vals[3]);
	n[2][1] = (vals[2] * nmle[2][1] + vals[3] * nmle[3][1]) / (vals[2] + vals[3]);
	n[3][0] = (vals[0] * nmle[0][0] + vals[3] * nmle[3][0]) / (vals[0] + vals[3]);
	n[3][1] = (vals[0] * nmle[0][1] + vals[3] * nmle[3][1]) / (vals[0] + vals[3]);
	n[4][0] = (vals[1] * nmle[1][0] + vals[5] * nmle[5][0]) / (vals[1] + vals[5]);
	n[4][1] = (vals[1] * nmle[1][1] + vals[5] * nmle[5][1]) / (vals[1] + vals[5]);
	n[5][0] = (vals[2] * nmle[2][0] + vals[6] * nmle[6][0]) / (vals[2] + vals[6]);
	n[5][1] = (vals[2] * nmle[2][1] + vals[6] * nmle[6][1]) / (vals[2] + vals[6]);
	n[6][0] = (vals[3] * nmle[3][0] + vals[7] * nmle[7][0]) / (vals[3] + vals[7]);
	n[6][1] = (vals[3] * nmle[3][1] + vals[7] * nmle[7][1]) / (vals[3] + vals[7]);
	n[7][0] = (vals[0] * nmle[0][0] + vals[4] * nmle[4][0]) / (vals[0] + vals[4]);
	n[7][1] = (vals[0] * nmle[0][1] + vals[4] * nmle[4][1]) / (vals[0] + vals[4]);
	n[8][0] = (vals[4] * nmle[4][0] + vals[5] * nmle[5][0]) / (vals[4] + vals[5]);
	n[8][1] = (vals[4] * nmle[4][1] + vals[5] * nmle[5][1]) / (vals[4] + vals[5]);
	n[9][0] = (vals[5] * nmle[5][0] + vals[6] * nmle[6][0]) / (vals[5] + vals[6]);
	n[9][1] = (vals[5] * nmle[5][1] + vals[6] * nmle[6][1]) / (vals[5] + vals[6]);
	n[10][0] = (vals[6] * nmle[6][0] + vals[7] * nmle[7][0]) / (vals[6] + vals[7]);
	n[10][1] = (vals[6] * nmle[6][1] + vals[7] * nmle[7][1]) / (vals[6] + vals[7]);
	n[11][0] = (vals[4] * nmle[4][0] + vals[7] * nmle[7][0]) / (vals[4] + vals[7]);
	n[11][1] = (vals[4] * nmle[4][1] + vals[7] * nmle[7][1]) / (vals[4] + vals[7]);

	n[0][2] = (vals[0] * nmle[0][2] + vals[1] * nmle[1][2]) / (vals[0] + vals[1]);
	n[1][2] = (vals[1] * nmle[1][2] + vals[2] * nmle[2][2]) / (vals[1] + vals[2]);
	n[2][2] = (vals[2] * nmle[2][2] + vals[3] * nmle[3][2]) / (vals[2] + vals[3]);
	n[3][2] = (vals[0] * nmle[0][2] + vals[3] * nmle[3][2]) / (vals[0] + vals[3]);
	n[4][2] = (vals[1] * nmle[1][2] + vals[5] * nmle[5][2]) / (vals[1] + vals[5]);
	n[5][2] = (vals[2] * nmle[2][2] + vals[6] * nmle[6][2]) / (vals[2] + vals[6]);
	n[6][2] = (vals[3] * nmle[3][2] + vals[7] * nmle[7][2]) / (vals[3] + vals[7]);
	n[7][2] = (vals[0] * nmle[0][2] + vals[4] * nmle[4][2]) / (vals[0] + vals[4]);
	n[8][2] = (vals[4] * nmle[4][2] + vals[5] * nmle[5][2]) / (vals[4] + vals[5]);
	n[9][2] = (vals[5] * nmle[5][2] + vals[6] * nmle[6][2]) / (vals[5] + vals[6]);
	n[10][2] = (vals[6] * nmle[6][2] + vals[7] * nmle[7][2]) / (vals[6] + vals[7]);
	n[11][2] = (vals[4] * nmle[4][2] + vals[7] * nmle[7][2]) / (vals[4] + vals[7]);

	for (int i = 0; i < 12; i++) {
		mag = sqrt(n[i][0] * n[i][0] + n[i][1] * n[i][1] + n[i][2] * n[i][2]);
		if (mag > 0.0f) {
			n[i][0] = n[i][0] / mag; n[i][1] = n[i][1] / mag; n[i][2] = n[i][2] / mag;
		}
	}

	int2 pix;
	// add new faces in the list
	int f = 0;
	for (f = 0; f < nb_faces; f++) {
		if (reverse) {
			Faces[3 * (offset + f)] = 3 * (offset + f) + 2;
			Faces[3 * (offset + f) + 1] = 3 * (offset + f) + 1;
			Faces[3 * (offset + f) + 2] = 3 * (offset + f);
		}
		else {
			Faces[3 * (offset + f)] = 3 * (offset + f);
			Faces[3 * (offset + f) + 1] = 3 * (offset + f) + 1;
			Faces[3 * (offset + f) + 2] = 3 * (offset + f) + 2;
		}

		int id_f_0 = Config[index][f][0];
		int id_f_1 = Config[index][f][1];
		int id_f_2 = Config[index][f][2];


		//float3 v0_v1 = (float3) { v[id_f_1][0] - v[id_f_0][0], v[id_f_1][1] - v[id_f_0][1], v[id_f_1][2] - v[id_f_0][2] };
		//float3 v0_v2 = (float3) { v[id_f_2][0] - v[id_f_0][0], v[id_f_2][1] - v[id_f_0][1], v[id_f_2][2] - v[id_f_0][2] };
		//float3 nrml = normalize(cross(v0_v1, v0_v2));


		VtxNmle[9 * (offset + f) + 0] = v[id_f_0][0];
		VtxNmle[9 * (offset + f) + 1] = v[id_f_0][1];
		VtxNmle[9 * (offset + f) + 2] = v[id_f_0][2];
		VtxNmle[9 * (offset + f) + 3] = v[id_f_1][0];
		VtxNmle[9 * (offset + f) + 4] = v[id_f_1][1];
		VtxNmle[9 * (offset + f) + 5] = v[id_f_1][2];
		VtxNmle[9 * (offset + f) + 6] = v[id_f_2][0];
		VtxNmle[9 * (offset + f) + 7] = v[id_f_2][1];
		VtxNmle[9 * (offset + f) + 8] = v[id_f_2][2];

		VtxNmle[9 * (offset + f) + 0 + stride] = n[id_f_0][0];
		VtxNmle[9 * (offset + f) + 1 + stride] = n[id_f_0][1];
		VtxNmle[9 * (offset + f) + 2 + stride] = n[id_f_0][2];
		VtxNmle[9 * (offset + f) + 3 + stride] = n[id_f_1][0];
		VtxNmle[9 * (offset + f) + 4 + stride] = n[id_f_1][1];
		VtxNmle[9 * (offset + f) + 5 + stride] = n[id_f_1][2];
		VtxNmle[9 * (offset + f) + 6 + stride] = n[id_f_2][0];
		VtxNmle[9 * (offset + f) + 7 + stride] = n[id_f_2][1];
		VtxNmle[9 * (offset + f) + 8 + stride] = n[id_f_2][2];

		////calib : 580.8857f, 583.317f, cDepthWidth / 2.0, cDepthHeight / 2.0
		//// Project onto Image
		////pix.x = convert_int(round((pt_T.x / fabs(pt_T.z))*calib[0] + calib[2]));
		////pix.y = n_row - convert_int(round((pt_T.y / fabs(pt_T.z))*calib[1] + calib[3]));

		//pix.x = convert_int(round( (v[id_f_0][0] / fabs(v[id_f_0][2])) * 580.8857f + 320.0f));
		//pix.y = convert_int(round( (v[id_f_0][1] / fabs(v[id_f_0][2])) * 583.3170f + 240.0f));

		////pix.x = 100; pix.y = 100;

		//uint4 pixel;

		//// Check if the pixel is in the frame
		//if (pix.x < 0 || pix.x > 639 || pix.y < 0 || pix.y > 479) {
		//	pixel.x = 1.0;
		//	pixel.y = 0.0;
		//	pixel.z = 0.0;
		//	pixel.w = 1.0;
		//}
		//else {
		//	int2 coords = (int2) { pix.x, pix.y };
		//	const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
		//	pixel = read_imageui(color, smp, coords);
		//}
		//VtxNmle[9 * (offset + f) + 0 + stride * 2] = convert_float((pixel.x));
		//VtxNmle[9 * (offset + f) + 1 + stride * 2] = 0.0;// convert_float((pixel.y));
		//VtxNmle[9 * (offset + f) + 2 + stride * 2] = 0.0;//convert_float((pixel.z));// 255.0;
		//VtxNmle[9 * (offset + f) + 3 + stride * 2] = 0.0;//convert_float(pixel.x);// / 255.0f;
		//VtxNmle[9 * (offset + f) + 4 + stride * 2] = 0.0;//convert_float(pixel.y);// / 255.0f;
		//VtxNmle[9 * (offset + f) + 5 + stride * 2] = 0.0;//convert_float(pixel.z);// / 255.0f;
		//VtxNmle[9 * (offset + f) + 6 + stride * 2] = 0.0;//convert_float(pixel.x);// / 255.0f;
		//VtxNmle[9 * (offset + f) + 7 + stride * 2] = 0.0;//convert_float(pixel.y);// / 255.0f;
		//VtxNmle[9 * (offset + f) + 8 + stride * 2] = 0.0;//convert_float(pixel.z);// / 255.0f;

		//continue;
	}


}


__kernel void MarchingCubesIndexing(__global float *TSDF, __global int *Offset, __global int *IndexVal, __constant int *Dim, const float iso, __global int *faces_counter) 
{
	int x = get_global_id(0); /*height*/
	int y = get_global_id(1); /*width*/

	int s[8][3] = { { x, y, 0 },{ x + 1,y , 0 },{ x + 1, y + 1, 0 },{ x, y + 1, 0 },
	{ x, y, 0 },{ x + 1, y, 0 },{ x + 1, y + 1, 0 },{ x, y + 1, 0 } };
	float vals[8] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

	int index;
	int id;
	int max_z = Dim[2] - 1;
	bool stop;
	int z = 0;	

	if (Dim[0] <= x+1 || Dim[1] <= y+1)return;
	float convVal = 0.05;


	for (z = 0; z <max_z; z++) { /*depth*/
		id = z + Dim[0] * y + Dim[0] * Dim[1] * x;
		//id = x + Dim[0] * y + Dim[0] * Dim[1] * z;

		// get the 8  current summits
		s[0][2] = z;
		s[1][2] = z;
		s[2][2] = z;
		s[3][2] = z;
		s[4][2] = z + 1;
		s[5][2] = z + 1;
		s[6][2] = z + 1;
		s[7][2] = z + 1;


		// get the values of the implicit function at the summits
		// [val_0 ... val_7]
		stop = false;
		int k = 0;
		for (k = 0; k < 8; k++) {
			vals[k] = (float)(TSDF[s[k][2] + Dim[2] * s[k][1] + Dim[2] * Dim[1] * s[k][0]]) / convVal;
			if (fabs(vals[k]) >= 1.0f) {
				IndexVal[id] = 0;
				stop = true;
				break;
			}
		}
		if (stop) {
			continue;
		}

		// get the index value corresponding to the implicit function
		if (vals[7] <= iso) {
			index = (int)(vals[0] > iso) +
				(int)(vals[1] > iso) * 2 +
				(int)(vals[2] > iso) * 4 +
				(int)(vals[3] > iso) * 8 +
				(int)(vals[4] > iso) * 16 +
				(int)(vals[5] > iso) * 32 +
				(int)(vals[6] > iso) * 64;
			IndexVal[id] = index;
		}
		else {
			index = (int)(vals[0] <= iso) +
				(int)(vals[1] <= iso) * 2 +
				(int)(vals[2] <= iso) * 4 +
				(int)(vals[3] <= iso) * 8 +
				(int)(vals[4] <= iso) * 16 +
				(int)(vals[5] <= iso) * 32 +
				(int)(vals[6] <= iso) * 64;
			IndexVal[id] = -index;
		}

		// get the corresponding configuration
		if (index == 0)
			continue;

		Offset[id] = atomic_add(faces_counter, ConfigCount[index]);
	}


}

__kernel void InitArray(__global int *Array_x, __global int *Array_y, __global int *Array_z, __global int *Weights,
	__global int *Normale_x, __global int *Normale_y, __global int *Normale_z,
	__global float * Vertices, __constant float *Param, __constant int *Dim, __global int *vertex_counter, const int nb_faces) {

	int x = get_global_id(0);
	int y = get_global_id(1);
	int work_size = get_global_size(1);

	int face_indx = x*work_size + y;

	if (face_indx > nb_faces - 1)
		return;

	float4 pt;
	int indx;
	int coord_i, coord_j, coord_k;
	for (int k = 0; k < 3; k++) {
		pt.x = Vertices[9 * face_indx + 3 * k];
		pt.y = Vertices[9 * face_indx + 3 * k + 1];
		pt.z = Vertices[9 * face_indx + 3 * k + 2];

		coord_i = max(0, min(Dim[0] - 1, (int)(round(pt.x*Param[1] + Param[0]))));
		coord_j = max(0, min(Dim[1] - 1, (int)(round(pt.y*Param[3] + Param[2]))));
		coord_k = max(0, min(Dim[2] - 1, (int)(round(pt.z*Param[5] + Param[4]))));

		indx = coord_i*Dim[2] * Dim[1] + coord_j*Dim[2] + coord_k;
		atomic_xchg(&Array_x[indx], 0);
		atomic_xchg(&Array_y[indx], 0);
		atomic_xchg(&Array_z[indx], 0);
		atomic_xchg(&Normale_x[indx], 0);
		atomic_xchg(&Normale_y[indx], 0);
		atomic_xchg(&Normale_z[indx], 0);
		atomic_xchg(&Weights[indx], 0);
	}

}

__kernel void MergeVtx(__global int *Array_x, __global int *Array_y, __global int *Array_z, __global int *Weights,
	__global int *Normale_x, __global int *Normale_y, __global int *Normale_z,
	__global int *VtxInd, __global float * Vertices, __global int *Faces, __constant float *Param,
	__constant int *Dim, __global int *vertex_counter, const int nb_faces) {

	int x = get_global_id(0);
	int y = get_global_id(1);
	int work_size = get_global_size(1);

	int face_indx = x*work_size + y;

	if (face_indx > nb_faces - 1)
		return;

	float4 pt[3];
	int indx[3];
	int flag;
	int counter;
	int coord_i, coord_j, coord_k;
	for (int k = 0; k < 3; k++) {
		pt[k].x = Vertices[3 * Faces[3 * face_indx + k]];
		pt[k].y = Vertices[3 * Faces[3 * face_indx + k] + 1];
		pt[k].z = Vertices[3 * Faces[3 * face_indx + k] + 2];

		coord_i = max(0, min(Dim[0] - 1, (int)(round(pt[k].x*Param[1] + Param[0]))));
		coord_j = max(0, min(Dim[1] - 1, (int)(round(pt[k].y*Param[3] + Param[2]))));
		coord_k = max(0, min(Dim[2] - 1, (int)(round(pt[k].z*Param[5] + Param[4]))));

		indx[k] = coord_i*Dim[2] * Dim[1] + coord_j*Dim[2] + coord_k;

		atomic_add(&Array_x[indx[k]], (int)(round(pt[k].x*100000.0f)));
		atomic_add(&Array_y[indx[k]], (int)(round(pt[k].y*100000.0f)));
		atomic_add(&Array_z[indx[k]], (int)(round(pt[k].z*100000.0f)));
		flag = atomic_inc(&Weights[indx[k]]);
		Faces[3 * face_indx + k] = indx[k];

		if (flag == 0) {
			counter = atomic_inc(vertex_counter);
			atomic_xchg(&VtxInd[indx[k]], counter);
		}
	}

	float4 v1 = (float4)(pt[1].x - pt[0].x, pt[1].y - pt[0].y, pt[1].z - pt[0].z, 1.0f);
	float4 v2 = (float4)(pt[2].x - pt[0].x, pt[2].y - pt[0].y, pt[2].z - pt[0].z, 1.0f);
	float4 nmle = (float4)(v1.y*v2.z - v1.z*v2.y,
		-v1.x*v2.z + v1.z*v2.x,
		v1.x*v2.y - v1.y*v2.x, 1.0f);
	for (int k = 0; k < 3; k++) {
		atomic_add(&Normale_x[indx[k]], (int)(round(nmle.x*100000.0f)));
		atomic_add(&Normale_y[indx[k]], (int)(round(nmle.y*100000.0f)));
		atomic_add(&Normale_z[indx[k]], (int)(round(nmle.z*100000.0f)));
	}



}


__kernel void SimplifyMesh(__global int *Array_x, __global int *Array_y, __global int *Array_z, __global int *Weights,
	__global int *Normale_x, __global int *Normale_y, __global int *Normale_z,
	__global int *VtxInd, __global float * Vertices, __global float * Normales, __global int *Faces,
	__constant int *Dim, const int nb_faces) {

	int x = get_global_id(0); /*height*/
	int y = get_global_id(1); /*width*/
	int work_size = get_global_size(1);

	int face_indx = x*work_size + y;

	if (face_indx > nb_faces - 1)
		return;

	int id;
	int id_v;
	float mag;
	for (int k = 0; k < 3; k++) {
		id = Faces[3 * face_indx + k];
		id_v = VtxInd[id];

		Vertices[3 * id_v] = ((float)(Array_x[id]) / 100000.0f) / (float)(Weights[id]);
		Vertices[3 * id_v + 1] = ((float)(Array_y[id]) / 100000.0f) / (float)(Weights[id]);
		Vertices[3 * id_v + 2] = ((float)(Array_z[id]) / 100000.0f) / (float)(Weights[id]);

		mag = sqrt((((float)(Normale_x[id])) / 100000.0f)*(((float)(Normale_x[id])) / 100000.0f) +
			(((float)(Normale_y[id])) / 100000.0f)*(((float)(Normale_y[id])) / 100000.0f) +
			(((float)(Normale_z[id])) / 100000.0f)*(((float)(Normale_z[id])) / 100000.0f));

		if (mag == 0.0f)
			mag = 1.0f;
		Normales[3 * id_v] = (((float)(Normale_x[id])) / 100000.0f) / mag;
		Normales[3 * id_v + 1] = (((float)(Normale_y[id])) / 100000.0f) / mag;
		Normales[3 * id_v + 2] = (((float)(Normale_z[id])) / 100000.0f) / mag;


		Faces[3 * face_indx + k] = id_v;
	}

}
