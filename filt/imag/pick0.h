/* This file is automatically generated. DO NOT EDIT! */

#ifndef _pick0_h
#define _pick0_h

void pick0_init (int n1_in, int n2_in /* dimensions */, 
		 int order            /* interpolation order */);
/*< Initialize >*/

void pick0_set (int i2, float* dip);
/*< set dip at trace i2 >*/

void pick0_close (void);
/*< Free allocated storage >*/

void pick0_step (float t0, float* t);
/*< step picking starting from t0 to t[n2] >*/

void pick0_step0 (float t0, float* t);
/*< step picking starting from t0 to t[n2] >*/

#endif
