__kernel void VmapKernel(__read_only image2d_t depth, __write_only image2d_t vmap, __constant float *calib, int Kinect, int n /*height*/, int m /*width*/) {

	unsigned int i = get_global_id(0); /*height*/
	unsigned int j = get_global_id(1); /*width*/
	unsigned int tid = i*m + j;
	int2 coords = (int2){get_global_id(1), get_global_id(0)};
	const sampler_t smp =  CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

	uint4 pixel = read_imageui(depth, smp, coords);
	
	float4 pt;
	float d;
	if (Kinect == 0) {
		pt.x = convert_float(pixel.x) / 6000.0f - 5.f;
		pt.y = convert_float(pixel.y) / 6000.0f - 5.f;
		pt.z = -convert_float(pixel.z) / 10000.0f, 0.0f;
	} else {
		d = convert_float(pixel.z);
		pt.x = d == 0.0 ? 0.0 : (calib[9] / calib[10]) * d * ((convert_float(j) - calib[2]) / calib[0]);
		pt.y = d == 0.0 ? 0.0 : -(calib[9] / calib[10]) * d * ((convert_float(i) - calib[3]) / calib[1]);
		//pt.y = d == 0.0 ? 0.0 : (calib[9] / calib[10]) * d * ((convert_float(n - 1 - i) - calib[3]) / calib[1]);
		pt.z = d == 0.0 ? 0.0 : -(calib[9] / calib[10]) * d;
		//pt.z = d == 0.0 ? 0.0 : (calib[9] / calib[10]) * d;
	}

	write_imagef(vmap, coords, pt);
}