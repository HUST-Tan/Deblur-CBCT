#include "mex.h"
#include "string.h"
#include "math.h"

#define Max(a,b) (a>b)?(a):(b)
#define Min(a,b) (a<b)?(a):(b)
#define Abs(a) (a>0)?(a):(-a)

#define root2 1.41421356
#define INTBYTES 4 
#define FLOATBYTES 4
#define eps 1e-18
#define Kernel 81
float AVR(int kpoint, int nx, int ny, int nz, int nu, int nv, int Nprj, float *d, float SAD, float SDD, float *us, float *vs, float *angle, float *var_1, float *res, float *l_ph0, float *l_theta0, int *column_num, float *column_data, float *Cos, float *Sin);
void updatew(float *w, float *u, int kpoint, int *object_size);
inline int iso_num(int kpoint, int kx, int ky, int kz, int nx, int ny, int nz, float *u);

void mexFunction(int nlhs, mxArray * plhs[], int nrhs, const mxArray *prhs[])
{
	float *u = (float *)mxGetPr(prhs[0]);
	double beta = (double)mxGetScalar(prhs[1]);
	//float *w=(float *)mxGetPr(prhs[2]);
	int *object_size = (int *)mxGetPr(prhs[3]);
	int *detect_size = (int *)mxGetPr(prhs[4]);
	int Nprj = (int)mxGetScalar(prhs[5]);
	float SDD = (float)mxGetScalar(prhs[6]);
	float SAD = (float)mxGetScalar(prhs[7]);
	float *d = (float*)mxGetPr(prhs[8]);
	float *us = (float*)mxGetPr(prhs[9]);
	float *vs = (float*)mxGetPr(prhs[10]);
	float *var_1 = (float *)mxGetPr(prhs[11]);
	float *r = (float *)mxGetPr(prhs[12]);
	float *s = (float *)mxGetPr(prhs[13]);
	float *angle = (float *)mxGetPr(prhs[14]);
	int *column_num = (int *)mxGetPr(prhs[15]);
	float *column_data = (float *)mxGetPr(prhs[16]);
    float *g = (float *)mxGetPr(prhs[17]);

	//float *g = (float *)mxGetPr(prhs[17]);

	int nx = object_size[0], ny = object_size[1], nz = object_size[2];
	int nxy = nx*ny, nxyz = nx*ny*nz;       
	int nu = detect_size[0], nv = detect_size[1];
	int nuTnv = nu*nv;
	int kx, ky, kz, ku, kv, nprj, data_num;

	register int i;
	int kpoint;
	float alpha, ph0;
	float sum = 0, sum_wu = 0, sum_w = 0;
	float ukold, uk;    //
	float w[24];
	float* l_ph0 = (float *)mxCalloc(nu*Nprj, sizeof(float));
	float *l_theta0 = (float *)mxCalloc(nuTnv, sizeof(float));
	float *Cos = (float *)mxCalloc(Nprj, sizeof(float));
	float *Sin = (float *)mxCalloc(Nprj, sizeof(float));
	float *avr = (float *)mxCalloc(Nprj, sizeof(float));
	// calculate cos和sin
    for (nprj = 0; nprj<Nprj; nprj++)
	{
		Cos[nprj] = cos(angle[nprj]);
		Sin[nprj] = sin(angle[nprj]);
	}
    // -----Part: amplitude：III(A)------
	for (nprj = 0; nprj<Nprj; nprj++)
	{
		for (ku = 0; ku<nu; ku++)
		{
			ph0 = angle[nprj] - atan(us[ku] / SDD);    // ph
			l_ph0[nprj*nu + ku] = d[0] / (Max((Abs(cos(ph0))), (Abs(sin(ph0)))));   // l_phi0
		}
	}

	for (kv = 0; kv<nv; kv++)
	{
		for (ku = 0; ku<nu; ku++)
		{
			l_theta0[kv*nu + ku] = sqrt(SDD*SDD + us[ku] * us[ku] + vs[kv] * vs[kv]) / sqrt(SDD*SDD + us[ku] * us[ku]);  // l_theta0
		}
	}
    //-------------------------------------
    // point-by-point
	for (kpoint = 7*nxy; kpoint<8*nxy; kpoint++){
		sum = 0;
		sum_w = 0;
		sum_wu = 0;
		kz = (kpoint) / (nxy)+1;
		ky = (kpoint - (kz - 1)*nxy) / nx + 1;
		kx = (kpoint) % nx + 1;
		ukold = u[kpoint];
		
		//alpha = s[kpoint] + beta*sum_w+beta;
		alpha = s[kpoint] +beta;   

		sum = AVR(kpoint, nx, ny, nz, nu, nv, Nprj, d, SAD, SDD, us, vs, angle, var_1, r, l_ph0, l_theta0, column_num, column_data, Cos, Sin);
		
		uk = (sum + s[kpoint] * ukold +beta*g[kpoint]) / alpha;
		uk = Max(uk, 0);
		uk = Min(uk, 0.03);
		if (s[kpoint]<1e-8)  { uk = ukold; }
		u[kpoint] = uk;

		data_num = column_num[0] + 1;
		for (i = 1; i<data_num; i++)
		{
			r[column_num[i]] += (ukold - uk)*(column_data[i]);
		} 
	}//forpoint
	//  mexPrintf("data_numt=%d\nkpoint=%d",data_num-1,kpoint);
}

float AVR(int kpoint, int nx, int ny, int nz, int nu, int nv, int Nprj, float *d, float SAD, float SDD, float *us, float *vs, float *angle, float *var_1, float *res, float *l_ph0, float *l_theta0, int *column_num, float *column_data, float *Cos, float *Sin)
{
	float avr_0 = 0;
	int nxy = nx*ny;
	int kz = (kpoint) / (nxy);
	int ky = (kpoint - (kz)*nxy) / nx;
	int kx = (kpoint) % nx;
	float dx = d[0], dy = d[1], dz = d[2];

	int i, j, k, ku, kv, kuv, nprj, nuTnv = nu*nv;
	float tu[4], tv[2], xs[3], ys[3], zs[2], beta, ph0, tu_t;
	int tu_sec[5], u[2], v[2];
	float F1_t, l, r, a, F1[5], F2;
	int count = 0, data_num = 0;
	float du = us[1] - us[0];
	float dv = vs[1] - vs[0];

	xs[0] = (-nx / 2 + kx)*dx;
	xs[1] = (-nx / 2 + kx + 1)*dx;
	xs[2] = (-nx / 2 + kx + 0.5)*dx;
	ys[0] = (-ny / 2 + ky)*dy;
	ys[1] = (-ny / 2 + ky + 1)*dy;
	ys[2] = (-ny / 2 + ky + 0.5)*dy;
	zs[0] = (-nz / 2 + kz)*dz;
	zs[1] = (-nz / 2 + kz + 1)*dz;
	//    nprj=0;
	for (nprj = 0; nprj<Nprj; nprj++)
	{
		count = 0;
		//    beta=angle[nprj];     
		tu[0] = SDD*(-xs[0] * Sin[nprj] + ys[0] * Cos[nprj]) / (SAD - xs[0] * Cos[nprj] - ys[0] * Sin[nprj]);
		tu[1] = SDD*(-xs[1] * Sin[nprj] + ys[0] * Cos[nprj]) / (SAD - xs[1] * Cos[nprj] - ys[0] * Sin[nprj]);
		tu[2] = SDD*(-xs[0] * Sin[nprj] + ys[1] * Cos[nprj]) / (SAD - xs[0] * Cos[nprj] - ys[1] * Sin[nprj]);
		tu[3] = SDD*(-xs[1] * Sin[nprj] + ys[1] * Cos[nprj]) / (SAD - xs[1] * Cos[nprj] - ys[1] * Sin[nprj]);

		for (i = 0; i<4; i++)
		{
			k = i;
			for (j = i + 1; j<4; j++)
			{
				if (tu[j]<tu[k])
				{
					k = j;
				}
			}
			if (k != i)
			{
				tu_t = tu[i];
				tu[i] = tu[k];
				tu[k] = tu_t;
			}
		}

		u[0] = (tu[0] - us[0]) / du - 1;
		if (u[0]<0)
		{
			u[0] = 0;
		}
		u[1] = (tu[3] - us[0]) / du + 1;
		if (u[1] >= nu)
		{
			u[1] = nu - 1;
		}

		for (ku = u[0]; ku <= u[1]; ku++)
		{
			if ((us[ku] - du / 2>tu[0] && us[ku] - du / 2<tu[3]) || (us[ku] + du / 2>tu[0] && us[ku] + du / 2<tu[3]))
			{
				F1_t = 0;
				l = Max(tu[0], us[ku] - du / 2);
				r = Min(tu[1], us[ku] + du / 2);
				if (l<r)
				{
					F1_t += ((r - tu[0])*(r - tu[0]) - (l - tu[0])*(l - tu[0])) / 2 / (tu[1] - tu[0]);
				}

				l = Max(tu[1], us[ku] - du / 2);
				r = Min(tu[2], us[ku] + du / 2);
				if (l<r)
				{
					F1_t += (r - l);
				}

				l = Max(tu[2], us[ku] - du / 2);
				r = Min(tu[3], us[ku] + du / 2);
				if (l<r)
				{
					F1_t += ((l - tu[3])*(l - tu[3]) - (r - tu[3])*(r - tu[3])) / 2 / (tu[3] - tu[2]);
				}
				F1[count] = F1_t / du;
				tu_sec[count] = ku;
				count++;
			}
		}

		if (count == 0){
			continue;
		}
		tv[0] = -zs[0] * SDD / (SAD - (xs[0] + xs[1]) / 2 * Cos[nprj] - (ys[0] + ys[1]) / 2 * Sin[nprj]);
		tv[1] = -zs[1] * SDD / (SAD - (xs[0] + xs[1]) / 2 * Cos[nprj] - (ys[0] + ys[1]) / 2 * Sin[nprj]);

		if (tv[1]<tv[0])
		{
			l = tv[0];
			tv[0] = tv[1];
			tv[1] = l;
		}

		v[0] = ((tv[0] - vs[0]) / dv) - 1;
		if (v[0]<0)
		{
			v[0] = 0;
		}
		v[1] = ((tv[1] - vs[0]) / dv) + 1;
		if (v[1] >= nv)
		{
			v[1] = nv - 1;
		}
		for (kv = v[0]; kv <= v[1]; kv++)
		{
			if ((vs[kv] - dv / 2>tv[0] && vs[kv] - dv / 2<tv[1]) || (vs[kv] + dv / 2>tv[0] && vs[kv] + dv / 2<tv[1]))
			{
				l = Max(tv[0], (vs[kv] - dv / 2));
				r = Min(tv[1], (vs[kv] + dv / 2));
				if (l<r)
				{
					F2 = (r - l) / dv;
					for (i = 0; i<count; i++)
					{
						ku = tu_sec[i];
						kuv = nprj*nuTnv + kv*nu + ku;
						data_num++;
						a = F1[i] * F2*l_ph0[nprj*nu + ku] * l_theta0[kv*nu + ku];
						column_data[data_num] = a;
						column_num[data_num] = kuv;
						avr_0 += a*var_1[kuv] * res[kuv];
					}
				}
			}
		}
	}// for nprj
	column_num[0] = data_num;
	return avr_0;
}