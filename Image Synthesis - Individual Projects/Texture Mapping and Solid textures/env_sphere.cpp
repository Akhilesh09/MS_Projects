
// Vector L=npe;
// // L.x=25;
// // L.y=90;
// // L.z=10;
// // L=L*(1/magnitude(L.x,L.y,L.z));

// float phi=acos(L.z);
// float v=phi/3.14;
// float x_prime=v*height;

// float theta=acos(L.y/sin(phi));

// // float u=theta/(2*3.14);

// float u=theta/(2*3.14);
// if(L.x<0)
// 	u=1-u;
// float y_prime=u*width;

// int k=((int)y_prime*width+(int)x_prime)*3;
// plane_arr_f[i] = env_arr[k];
// // plane_arr_f[i] = plane_arr_f[i]*(1-S)+255*(S);
// // plane_arr_f[i] = plane_arr_f[i]*(1-B)+255*(B);
// plane_arr_f[i+1] = env_arr[k+1];
// // plane_arr_f[i+1] = plane_arr_f[i+1]*(1-S)+255*(S);
// // plane_arr_f[i+1] = plane_arr_f[i+1]*(1-B)+0*(B);
// plane_arr_f[i+2] = env_arr[k+2];
// // plane_arr_f[i+2] = plane_arr_f[i+2]*(1-S)+255*(S);
// // plane_arr_f[i+2] = plane_arr_f[i+2]*(1-B)+0*(B);