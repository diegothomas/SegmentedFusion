__kernel void FuseTSDF(__global float *TSDF,  __global float *Weight, __constant float *Param, __constant int *Dim,
	__constant float *Pose,	__constant float *boneDQ, __constant float *jointDQ, __constant float *planeF,
	__constant float *calib, const int n_row, const int m_col
	,__read_only image2d_t Depth
) {
	//const sampler_t smp =  CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

	const float nu = 0.05f;
	float4 pt;
	float4 ctr;
	float4 pt_T;
	float4 ctr_T;
	int2 pix;

	int x = get_global_id(0); /*height*/
	int y = get_global_id(1); /*width*/
	if (x > Dim[0] - 1 || y > Dim[1] - 1)return;


	pt.x = ((float)(x)-Param[0]) / Param[1];
	pt.y = ((float)(y)-Param[2]) / Param[3];
	float x_T = Pose[0] * pt.x + Pose[1] * pt.y + Pose[3];
	float y_T = Pose[4] * pt.x + Pose[5] * pt.y + Pose[7];
	float z_T = Pose[8] * pt.x + Pose[9] * pt.y + Pose[11];
	float w_T = Pose[12] * pt.x + Pose[13] * pt.y + Pose[15];
	//int to float?
	float convVal = 0.05;//32767.0f;
	//convVal = 32767.0f;
	int z;

	for (z = 0; z < Dim[2]; z++) { /*depth*/
								   // On the GPU all pixel are stocked in a 1D array
		int idx = z + Dim[2] * y + Dim[2] * Dim[1] * x;
		//idx = x + Dim[0] * y + Dim[0] * Dim[1] * z;
		//idx = (y * Dim[2] * Dim[1] + x);
		//TSDF[idx] = 1.0;
		// Transform voxel coordinates into 3D point coordinates
		// Param = [c_x, dim_x, c_y, dim_y, c_z, dim_z]
		pt.z = ((float)(z)-Param[4]) / Param[5];

		// Transfom the voxel into the Image coordinate space
		//transform form local to global
		pt_T.x = x_T + Pose[2] * pt.z; //Pose is column major
		pt_T.y = y_T + Pose[6] * pt.z;
		pt_T.z = z_T + Pose[10] * pt.z;
		pt_T.w = w_T + Pose[14] * pt.z;

		//float mmmm = sqrt((Dim[0] / 2.0 - x)*(Dim[0] / 2.0 - x) + (Dim[1] / 2.0 - y)*(Dim[1] / 2.0 - y) + (Dim[2] / 2.0 - z)*(Dim[2] / 2.0 - z));
		//if (mmmm < 10.0)	TSDF[idx] = -convVal + 0.001;
		//else TSDF[idx] = convVal*0.001;
		//continue;

		//transform from first frame to current frame according interploation
		float weight = 0.0;
		float weightspara = 0.04;
		//weightspara = 0.02;


		weight = (planeF[0] * pt_T.x + planeF[1] * pt_T.y + planeF[2] * pt_T.z + planeF[3]);
		//weight = (planeF[0] * pt.x + planeF[1] * pt.y + planeF[2] * pt.z + planeF[3]);
		if (weight <= 0) {
			weight = 1.0;
		}
		else {
			//weight = exp(-weight*weight / 2.0 / weightspara / weightspara);
			weight = exp(-weight/weightspara);
		}
		//weight = exp(-(float)x / (float)Dim[0]);
		//weight = 0.0;

		float DQ[2][4];
		DQ[0][0] = (1.0 - weight)*boneDQ[0 * 4 + 0];
		DQ[0][1] = (1.0 - weight)*boneDQ[0 * 4 + 1];
		DQ[0][2] = (1.0 - weight)*boneDQ[0 * 4 + 2];
		DQ[0][3] = (1.0 - weight)*boneDQ[0 * 4 + 3];
		DQ[1][1] = (1.0 - weight)*boneDQ[1 * 4 + 1];
		DQ[1][2] = (1.0 - weight)*boneDQ[1 * 4 + 2];
		DQ[1][3] = (1.0 - weight)*boneDQ[1 * 4 + 3];
		DQ[0][0] += (weight)*jointDQ[0 * 4 + 0];
		DQ[0][1] += (weight)*jointDQ[0 * 4 + 1];
		DQ[0][2] += (weight)*jointDQ[0 * 4 + 2];
		DQ[0][3] += (weight)*jointDQ[0 * 4 + 3];
		DQ[1][1] += (weight)*jointDQ[1 * 4 + 1];
		DQ[1][2] += (weight)*jointDQ[1 * 4 + 2];
		DQ[1][3] += (weight)*jointDQ[1 * 4 + 3];
		float mag = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] + DQ[0][2] * DQ[0][2] + DQ[0][3] * DQ[0][3];
		mag = pow(mag, 0.5f);
		if (mag != 0) {
			DQ[0][0] = DQ[0][0] / mag;
			DQ[0][1] = DQ[0][1] / mag;
			DQ[0][2] = DQ[0][2] / mag;
			DQ[0][3] = DQ[0][3] / mag;
			DQ[1][1] = DQ[1][1]/ mag;
			DQ[1][2] = DQ[1][2]/ mag;
			DQ[1][3] = DQ[1][3]/ mag;
		}
		else {
			DQ[0][0] = 1.0;
			DQ[0][1] = 0.0;
			DQ[0][2] = 0.0;
			DQ[0][3] = 0.0;
			DQ[1][1] = 0.0;
			DQ[1][2] = 0.0;
			DQ[1][3] = 0.0;
		}

		//get matrix from DQ
		float Tr[4][4];
		Tr[0][0] = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2] - DQ[0][3] * DQ[0][3];
		Tr[1][0] = 2.0*DQ[0][1] * DQ[0][2] + 2.0*DQ[0][0] * DQ[0][3];
		Tr[2][0] = 2.0*DQ[0][1] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][2];
		Tr[0][1] = 2.0*DQ[0][1] * DQ[0][2] - 2.0*DQ[0][0] * DQ[0][3];
		Tr[1][1] = DQ[0][0] * DQ[0][0] + DQ[0][2] * DQ[0][2] - DQ[0][1] * DQ[0][1] - DQ[0][3] * DQ[0][3];
		Tr[2][1] = 2.0*DQ[0][2] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][1];
		Tr[0][2] = 2.0*DQ[0][1] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][2];
		Tr[1][2] = 2.0*DQ[0][2] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][1];
		Tr[2][2] = DQ[0][0] * DQ[0][0] + DQ[0][3] * DQ[0][3] - DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2];
		Tr[0][3] = -2.0*DQ[1][2] * DQ[0][3] + 2.0*DQ[1][3] * DQ[0][2] + 2.0*DQ[1][1] * DQ[0][0];
		Tr[1][3] = -2.0*DQ[1][3] * DQ[0][1] + 2.0*DQ[1][1] * DQ[0][3] + 2.0*DQ[1][2] * DQ[0][0];
		Tr[2][3] = -2.0*DQ[1][1] * DQ[0][2] + 2.0*DQ[1][2] * DQ[0][1] + 2.0*DQ[1][3] * DQ[0][0];
		Tr[3][0] = Tr[3][1] = Tr[3][2] = 0.0;
		Tr[3][3] = 1.0;
		pt = pt_T;
		pt.x = Tr[0][0] * pt_T.x + Tr[0][1] * pt_T.y + Tr[0][2] * pt_T.z + Tr[0][3];
		pt.y = Tr[1][0] * pt_T.x + Tr[1][1] * pt_T.y + Tr[1][2] * pt_T.z + Tr[1][3];
		pt.z = Tr[2][0] * pt_T.x + Tr[2][1] * pt_T.y + Tr[2][2] * pt_T.z + Tr[2][3];
		pt.w = Tr[3][0] * pt_T.x + Tr[3][1] * pt_T.y + Tr[3][2] * pt_T.z + Tr[3][3];


		pt_T.x = pt.x / pt.w;
		pt_T.y = pt.y / pt.w;
		pt_T.z = pt.z / pt.w;
		pt_T.w = pt.w / pt.w;

		// Project onto Image
		pix.x = convert_int(round((pt_T.x / fabs(pt_T.z))*calib[0] + calib[2]));
		pix.y = n_row - convert_int(round((pt_T.y / fabs(pt_T.z))*calib[1] + calib[3]));
		pix.y = convert_int(round((pt_T.y / fabs(pt_T.z))*calib[1] + calib[3]));

		// Check if the pixel is in the frame
		if (pix.x < 0 || pix.x > m_col - 1 || pix.y < 0 || pix.y > n_row - 1) {
			//if (Weight[idx] == 0.0)
			TSDF[idx] = convVal;
			continue;
		}

		//Compute distance between project voxel and surface in the RGBD image
		int2 coords = (int2) { pix.x,pix.y };
		const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

		//float4 color;
		//color.x = weight; color.y = weight; color.z = 1.0; color.w = 1.0;
		//write_imagef(mask, coords, color);

		uint4 pixel = read_imageui(Depth, smp, coords);
		float d = convert_float(pixel.z);
		//float dist = -(float)(-pt_T.z - (float)(d* (calib[9] / calib[10]))) / convVal;//nu;
		float dist = -(float)(pt_T.z - (float)(d* (calib[9] / calib[10]))) / convVal;//nu;
		dist = min(1.0f, max(-1.0f, dist));

		if (d == 0.0f) {
			Weight[idx] = max(0.0, Weight[idx] - 1.0);
			if (Weight[idx] == 0.0f) {
				TSDF[idx] =  convVal;
			}
			//TSDF[idx] = (convVal);
			continue;
		}

		//if ((float)(d* (calib[9] / calib[10])) >= -pt_T.z+0.2 || d == 0.0) {
		//	Weight[idx] = max(0.0, Weight[idx] - 3.0);
		//	if (Weight[idx] == 0.0) {
		//		TSDF[idx] = convVal;
		//	}
		//	continue;
		//}

		//if (dist == -1.0f) {
		//	//if (Weight[idx] == 0.0) {
		//	TSDF[idx] = -1.0f *convVal;
		//	//}
		//	continue;
		//}

		//float w = 1.0f;
		//if (dist < (float)(TSDF[idx]) / convVal) w = 0.1f;
		//if (dist > 1.0f) dist = 1.0f;
		//else dist = max(-1.0f, dist);


		// Running Average
		float prev_tsdf = (float)(TSDF[idx]) / convVal;
		float prev_weight = Weight[idx];

		TSDF[idx] =(((prev_tsdf*prev_weight + dist) / (prev_weight + 1.0f))*convVal);
		//TSDF[idx] = dist *convVal;
		Weight[idx] = min(50.0f, Weight[idx] + 1.0f);
	}

}


__kernel void Transform_color(__global float * Vertices, __constant float *Pose, __constant float *boneDQ, 
	__constant float *jointDQ, __constant float *planeF, 
	__constant int *Dim, __global int *faces_counter
	,__read_only image2d_t color, __constant float *PoseD2C
)
{
	int i = get_global_id(0); /**/
	int j = get_global_id(1); /**/

	int x = i%Dim[0];
	int y = i / Dim[0];
	int z = j;

	int idx = x + Dim[0] * y + Dim[0] * Dim[1] * z;
	idx = z + Dim[0] * y + Dim[0] * Dim[1] * x;
	int stride = Dim[0] * Dim[1] * Dim[2] *9;
	float4 pt;
	float4 pt_T;
	pt.x = Vertices[idx * 3 + 0];
	pt.y = Vertices[idx * 3 + 1];
	pt.z = Vertices[idx * 3 + 2];
	pt.w = 1.0;

	float x_T = Pose[0] * pt.x + Pose[1] * pt.y + Pose[3];
	float y_T = Pose[4] * pt.x + Pose[5] * pt.y + Pose[7];
	float z_T = Pose[8] * pt.x + Pose[9] * pt.y + Pose[11];
	float w_T = Pose[12] * pt.x + Pose[13] * pt.y + Pose[15];


	// Transfom the voxel into the Image coordinate space
	//transform form local to global
	pt_T.x = x_T + Pose[2] * pt.z; //Pose is column major
	pt_T.y = y_T + Pose[6] * pt.z;
	pt_T.z = z_T + Pose[10] * pt.z;
	pt_T.w = w_T + Pose[14] * pt.z;

	float4 nmle;
	float4 nmle_T;
	nmle.x = Vertices[idx * 3 + 0 + stride];
	nmle.y = Vertices[idx * 3 + 1 + stride];
	nmle.z = Vertices[idx * 3 + 2 + stride];
	nmle.w = 0.0;

	nmle_T.x = Pose[0] * nmle.x + Pose[1] * nmle.y + Pose[2] * nmle.z;
	nmle_T.y = Pose[4] * nmle.x + Pose[5] * nmle.y + Pose[6] * nmle.z;
	nmle_T.z = Pose[8] * nmle.x + Pose[9] * nmle.y + Pose[10] * nmle.z;
	nmle_T.w = 0.0;


	//transform from first frame to current frame according interploation
	float weight = 0.0;
	float weightspara = 0.04;
	//weightspara = 0.02;
	weight = (planeF[0] * pt_T.x + planeF[1] * pt_T.y + planeF[2] * pt_T.z + planeF[3]);
	//weight = (planeF[0] * pt.x + planeF[1] * pt.y + planeF[2] * pt.z + planeF[3]);


	if (weight < 0.0) {
		weight = 1.0;
	}
	else {
		//weight = exp(-weight*weight / 2.0 / weightspara / weightspara);
		weight = exp(-weight/weightspara);

	}

	float DQ[2][4];
	DQ[0][0] = (1.0 - weight)*boneDQ[0 * 4 + 0];
	DQ[0][1] = (1.0 - weight)*boneDQ[0 * 4 + 1];
	DQ[0][2] = (1.0 - weight)*boneDQ[0 * 4 + 2];
	DQ[0][3] = (1.0 - weight)*boneDQ[0 * 4 + 3];
	DQ[1][1] = (1.0 - weight)*boneDQ[1 * 4 + 1];
	DQ[1][2] = (1.0 - weight)*boneDQ[1 * 4 + 2];
	DQ[1][3] = (1.0 - weight)*boneDQ[1 * 4 + 3];
	DQ[0][0] += (weight)*jointDQ[0 * 4 + 0];
	DQ[0][1] += (weight)*jointDQ[0 * 4 + 1];
	DQ[0][2] += (weight)*jointDQ[0 * 4 + 2];
	DQ[0][3] += (weight)*jointDQ[0 * 4 + 3];
	DQ[1][1] += (weight)*jointDQ[1 * 4 + 1];
	DQ[1][2] += (weight)*jointDQ[1 * 4 + 2];
	DQ[1][3] += (weight)*jointDQ[1 * 4 + 3];
	float mag = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] + DQ[0][2] * DQ[0][2] + DQ[0][3] * DQ[0][3];
	mag = pow(mag, 0.5f);
	if (mag != 0.0) {
		DQ[0][0] = DQ[0][0] / mag;
		DQ[0][1] = DQ[0][1] / mag;
		DQ[0][2] = DQ[0][2] / mag;
		DQ[0][3] = DQ[0][3] / mag;
		DQ[1][1] = DQ[1][1]/ mag;
		DQ[1][2] = DQ[1][2]/ mag;
		DQ[1][3] = DQ[1][3]/ mag;
	}
	else {
		DQ[0][0] = 1.0;
		DQ[0][1] = 0.0;
		DQ[0][2] = 0.0;
		DQ[0][3] = 0.0;
		DQ[1][1] = 0.0;
		DQ[1][2] = 0.0;
		DQ[1][3] = 0.0;
	}

	//get matrix from DQ
	float Tr[4][4];
	Tr[0][0] = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2] - DQ[0][3] * DQ[0][3];
	Tr[1][0] = 2.0*DQ[0][1] * DQ[0][2] + 2.0*DQ[0][0] * DQ[0][3];
	Tr[2][0] = 2.0*DQ[0][1] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][2];
	Tr[0][1] = 2.0*DQ[0][1] * DQ[0][2] - 2.0*DQ[0][0] * DQ[0][3];
	Tr[1][1] = DQ[0][0] * DQ[0][0] + DQ[0][2] * DQ[0][2] - DQ[0][1] * DQ[0][1] - DQ[0][3] * DQ[0][3];
	Tr[2][1] = 2.0*DQ[0][2] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][1];
	Tr[0][2] = 2.0*DQ[0][1] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][2];
	Tr[1][2] = 2.0*DQ[0][2] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][1];
	Tr[2][2] = DQ[0][0] * DQ[0][0] + DQ[0][3] * DQ[0][3] - DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2];
	Tr[0][3] = -2.0*DQ[1][2] * DQ[0][3] + 2.0*DQ[1][3] * DQ[0][2] + 2.0*DQ[1][1] * DQ[0][0];
	Tr[1][3] = -2.0*DQ[1][3] * DQ[0][1] + 2.0*DQ[1][1] * DQ[0][3] + 2.0*DQ[1][2] * DQ[0][0];
	Tr[2][3] = -2.0*DQ[1][1] * DQ[0][2] + 2.0*DQ[1][2] * DQ[0][1] + 2.0*DQ[1][3] * DQ[0][0];
	Tr[3][0] = Tr[3][1] = Tr[3][2] = 0.0;
	Tr[3][3] = 1.0;

	pt.x = Tr[0][0] * pt_T.x + Tr[0][1] * pt_T.y + Tr[0][2] * pt_T.z + Tr[0][3];
	pt.y = Tr[1][0] * pt_T.x + Tr[1][1] * pt_T.y + Tr[1][2] * pt_T.z + Tr[1][3];
	pt.z = Tr[2][0] * pt_T.x + Tr[2][1] * pt_T.y + Tr[2][2] * pt_T.z + Tr[2][3];
	pt.w = Tr[3][0] * pt_T.x + Tr[3][1] * pt_T.y + Tr[3][2] * pt_T.z + Tr[3][3];

	pt_T.x = pt.x / pt.w;
	pt_T.y = pt.y / pt.w;
	pt_T.z = pt.z / pt.w;
	pt_T.w = pt.w / pt.w;

	nmle = nmle_T;
	nmle.x = Tr[0][0] * nmle_T.x + Tr[0][1] * nmle_T.y + Tr[0][2] * nmle_T.z;
	nmle.y = Tr[1][0] * nmle_T.x + Tr[1][1] * nmle_T.y + Tr[1][2] * nmle_T.z;
	nmle.z = Tr[2][0] * nmle_T.x + Tr[2][1] * nmle_T.y + Tr[2][2] * nmle_T.z;
	nmle.w = 0.0;

	nmle = normalize(nmle);

	Vertices[idx * 3 + 0] = pt_T.x;
	Vertices[idx * 3 + 1] = pt_T.y;
	Vertices[idx * 3 + 2] = pt_T.z;

	Vertices[idx * 3 + 0 + stride] = nmle.x;
	Vertices[idx * 3 + 1 + stride] = nmle.y;
	Vertices[idx * 3 + 2 + stride] = nmle.z;

	Vertices[idx * 3 + 0 + stride * 2] = 1.0;//nmle.x;
	Vertices[idx * 3 + 1 + stride * 2] = 1.0;//nmle.y;
	Vertices[idx * 3 + 2 + stride * 2] = 1.0;//nmle.z;


	return;

	// depth to camera transform
	pt = pt_T;
	pt_T.x = PoseD2C[0] * pt.x + PoseD2C[1] * pt.y + PoseD2C[2] * pt.z + PoseD2C[3];
	pt_T.y = PoseD2C[4] * pt.x + PoseD2C[5] * pt.y + PoseD2C[6] * pt.z + PoseD2C[7];
	pt_T.z = PoseD2C[8] * pt.x + PoseD2C[9] * pt.y + PoseD2C[10]* pt.z + PoseD2C[11];

	int2 pix;
	pix.x = convert_int(round(pt_T.x/fabs(pt_T.z) * 580.8857f) + 320.0f);// koko ga okashii 
	pix.y = convert_int(round(pt_T.y/fabs(pt_T.z) * 583.3170f) + 240.0f);// kotti ha soreppoi atai deteru

	//pix.x = 300;
	//pix.y = 350;

	uint4 pixel;

	// Check if the pixel is in the frame
	if (pix.x < 0 || pix.x > 639 || pix.y < 0 || pix.y > 479) {
	//if (false) {
		pixel.x = 1.0;
		pixel.y = 0.0;
		pixel.z = 0.0;
		pixel.w = 1.0;
	}
	else {
		int2 coords = (int2) { pix.x, pix.y };
		const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
		pixel = read_imageui(color, smp, coords);
	}

	Vertices[idx * 3 + 0 + stride * 2] = convert_float(pixel.z)/ 255.0;
	Vertices[idx * 3 + 1 + stride * 2] = convert_float(pixel.y)/ 255.0;
	Vertices[idx * 3 + 2 + stride * 2] = convert_float(pixel.x)/ 255.0;
}

__kernel void Transform(__global float * Vertices, __constant float *Pose, __constant float *boneDQ,
	__constant float *jointDQ, __constant float *planeF,
	__constant int *Dim, __global int *faces_counter
)
{
	int i = get_global_id(0); /**/
	int j = get_global_id(1); /**/

	int x = i%Dim[0];
	int y = i / Dim[0];
	int z = j;

	int idx = x + Dim[0] * y + Dim[0] * Dim[1] * z;
	idx = z + Dim[0] * y + Dim[0] * Dim[1] * x;
	int stride = Dim[0] * Dim[1] * Dim[2] * 9;
	float4 pt;
	float4 pt_T;
	pt.x = Vertices[idx * 3 + 0];
	pt.y = Vertices[idx * 3 + 1];
	pt.z = Vertices[idx * 3 + 2];
	pt.w = 1.0;

	float x_T = Pose[0] * pt.x + Pose[1] * pt.y + Pose[3];
	float y_T = Pose[4] * pt.x + Pose[5] * pt.y + Pose[7];
	float z_T = Pose[8] * pt.x + Pose[9] * pt.y + Pose[11];
	float w_T = Pose[12] * pt.x + Pose[13] * pt.y + Pose[15];


	// Transfom the voxel into the Image coordinate space
	//transform form local to global
	pt_T.x = x_T + Pose[2] * pt.z; //Pose is column major
	pt_T.y = y_T + Pose[6] * pt.z;
	pt_T.z = z_T + Pose[10] * pt.z;
	pt_T.w = w_T + Pose[14] * pt.z;

	float4 nmle;
	float4 nmle_T;
	nmle.x = Vertices[idx * 3 + 0 + stride];
	nmle.y = Vertices[idx * 3 + 1 + stride];
	nmle.z = Vertices[idx * 3 + 2 + stride];
	nmle.w = 0.0;

	nmle_T.x = Pose[0] * nmle.x + Pose[1] * nmle.y + Pose[2] * nmle.z;
	nmle_T.y = Pose[4] * nmle.x + Pose[5] * nmle.y + Pose[6] * nmle.z;
	nmle_T.z = Pose[8] * nmle.x + Pose[9] * nmle.y + Pose[10] * nmle.z;
	nmle_T.w = 0.0;


	//transform from first frame to current frame according interploation
	float weight = 0.0;
	float weightspara = 0.04;

	weight = (planeF[0] * pt_T.x + planeF[1] * pt_T.y + planeF[2] * pt_T.z + planeF[3]);

	if (weight < 0.0) {
		weight = 1.0;
	}
	else {
		//weight = exp(-weight*weight / 2.0 / weightspara / weightspara);
		weight = exp(-weight / weightspara);

	}

	float DQ[2][4];
	DQ[0][0] = (1.0 - weight)*boneDQ[0 * 4 + 0];
	DQ[0][1] = (1.0 - weight)*boneDQ[0 * 4 + 1];
	DQ[0][2] = (1.0 - weight)*boneDQ[0 * 4 + 2];
	DQ[0][3] = (1.0 - weight)*boneDQ[0 * 4 + 3];
	DQ[1][1] = (1.0 - weight)*boneDQ[1 * 4 + 1];
	DQ[1][2] = (1.0 - weight)*boneDQ[1 * 4 + 2];
	DQ[1][3] = (1.0 - weight)*boneDQ[1 * 4 + 3];
	DQ[0][0] += (weight)*jointDQ[0 * 4 + 0];
	DQ[0][1] += (weight)*jointDQ[0 * 4 + 1];
	DQ[0][2] += (weight)*jointDQ[0 * 4 + 2];
	DQ[0][3] += (weight)*jointDQ[0 * 4 + 3];
	DQ[1][1] += (weight)*jointDQ[1 * 4 + 1];
	DQ[1][2] += (weight)*jointDQ[1 * 4 + 2];
	DQ[1][3] += (weight)*jointDQ[1 * 4 + 3];
	float mag = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] + DQ[0][2] * DQ[0][2] + DQ[0][3] * DQ[0][3];
	mag = pow(mag, 0.5f);
	if (mag != 0.0) {
		DQ[0][0] = DQ[0][0] / mag;
		DQ[0][1] = DQ[0][1] / mag;
		DQ[0][2] = DQ[0][2] / mag;
		DQ[0][3] = DQ[0][3] / mag;
		DQ[1][1] = DQ[1][1] / mag;
		DQ[1][2] = DQ[1][2] / mag;
		DQ[1][3] = DQ[1][3] / mag;
	}
	else {
		DQ[0][0] = 1.0;
		DQ[0][1] = 0.0;
		DQ[0][2] = 0.0;
		DQ[0][3] = 0.0;
		DQ[1][1] = 0.0;
		DQ[1][2] = 0.0;
		DQ[1][3] = 0.0;
	}

	//get matrix from DQ
	float Tr[4][4];
	Tr[0][0] = DQ[0][0] * DQ[0][0] + DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2] - DQ[0][3] * DQ[0][3];
	Tr[1][0] = 2.0*DQ[0][1] * DQ[0][2] + 2.0*DQ[0][0] * DQ[0][3];
	Tr[2][0] = 2.0*DQ[0][1] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][2];
	Tr[0][1] = 2.0*DQ[0][1] * DQ[0][2] - 2.0*DQ[0][0] * DQ[0][3];
	Tr[1][1] = DQ[0][0] * DQ[0][0] + DQ[0][2] * DQ[0][2] - DQ[0][1] * DQ[0][1] - DQ[0][3] * DQ[0][3];
	Tr[2][1] = 2.0*DQ[0][2] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][1];
	Tr[0][2] = 2.0*DQ[0][1] * DQ[0][3] + 2.0*DQ[0][0] * DQ[0][2];
	Tr[1][2] = 2.0*DQ[0][2] * DQ[0][3] - 2.0*DQ[0][0] * DQ[0][1];
	Tr[2][2] = DQ[0][0] * DQ[0][0] + DQ[0][3] * DQ[0][3] - DQ[0][1] * DQ[0][1] - DQ[0][2] * DQ[0][2];
	Tr[0][3] = -2.0*DQ[1][2] * DQ[0][3] + 2.0*DQ[1][3] * DQ[0][2] + 2.0*DQ[1][1] * DQ[0][0];
	Tr[1][3] = -2.0*DQ[1][3] * DQ[0][1] + 2.0*DQ[1][1] * DQ[0][3] + 2.0*DQ[1][2] * DQ[0][0];
	Tr[2][3] = -2.0*DQ[1][1] * DQ[0][2] + 2.0*DQ[1][2] * DQ[0][1] + 2.0*DQ[1][3] * DQ[0][0];
	Tr[3][0] = Tr[3][1] = Tr[3][2] = 0.0;
	Tr[3][3] = 1.0;

	//pt = pt_T;
	pt.x = Tr[0][0] * pt_T.x + Tr[0][1] * pt_T.y + Tr[0][2] * pt_T.z + Tr[0][3];
	pt.y = Tr[1][0] * pt_T.x + Tr[1][1] * pt_T.y + Tr[1][2] * pt_T.z + Tr[1][3];
	pt.z = Tr[2][0] * pt_T.x + Tr[2][1] * pt_T.y + Tr[2][2] * pt_T.z + Tr[2][3];
	pt.w = Tr[3][0] * pt_T.x + Tr[3][1] * pt_T.y + Tr[3][2] * pt_T.z + Tr[3][3];

	pt_T.x = pt.x / pt.w;
	pt_T.y = pt.y / pt.w;
	pt_T.z = pt.z / pt.w;
	pt_T.w = pt.w / pt.w;

	nmle = nmle_T;
	nmle.x = Tr[0][0] * nmle_T.x + Tr[0][1] * nmle_T.y + Tr[0][2] * nmle_T.z;
	nmle.y = Tr[1][0] * nmle_T.x + Tr[1][1] * nmle_T.y + Tr[1][2] * nmle_T.z;
	nmle.z = Tr[2][0] * nmle_T.x + Tr[2][1] * nmle_T.y + Tr[2][2] * nmle_T.z;
	nmle.w = 0.0;

	nmle = normalize(nmle);

	Vertices[idx * 3 + 0] = pt_T.x;
	Vertices[idx * 3 + 1] = -pt_T.y;
	Vertices[idx * 3 + 2] = -pt_T.z;

	Vertices[idx * 3 + 0 + stride] = nmle.x;
	Vertices[idx * 3 + 1 + stride] = nmle.y;
	Vertices[idx * 3 + 2 + stride] = nmle.z;
}

