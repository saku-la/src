/* DSR modeling/migration in v(z) */
/*
  Copyright (C) 2004 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <math.h>

#include <rsf.h>

#include "dsr.h"
#include "pshift.h"

static float eps, *vt, dw;
static int nz, nw;
static float complex *pp;
static kiss_fftr_cfg forw, invs;

void dsr_init (float eps1 /* regularization */, 
	       int nt     /* time samples */, 
	       float dt   /* time sampling */, 
	       int nz1    /* depth samples */, 
	       float dz   /* depth sampling */, 
	       float *vt1 /* velocity/slowness */, 
	       bool depth /* depth or time migration */,
	       char rule   /* interpolation rule */)
/*< initialize >*/
{
    eps = eps1;     nz = nz1; 
    vt = vt1;

    /* determine frequency sampling */
    nw = nt/2+1;
    dw = 2.0*SF_PI/(nt*dt);
    
    forw = kiss_fftr_alloc(nt,0,NULL,NULL);
    invs = kiss_fftr_alloc(nt,1,NULL,NULL);
    
    /* allocate workspace */
    pp = sf_complexalloc (nw);
    pshift_init(depth,0.5*dz,rule);
} 

void dsr_close ()
/*< free workspace >*/
{    
    free (pp);  
    free (forw);
    free (invs);
}

void dsr (bool inv /* modeling or migration */, 
	  float kx /* midpoint wavenumber */, 
	  float kh /* half-offset wavenumber */, 
	  float *p /* time trace */, 
	  float *q /* depth trace */)
/*< apply >*/
{
    int iz,iw;
    float s, r;
    float complex w2;

    s = 0.5*(kx-kh);
    r = 0.5*(kx+kh);
    s *= s;
    r *= r;

    if (inv) { /* modeling */
	for (iw=0; iw<nw; iw++) {
	    pp[iw] = q[nz-1];
	}
	
	/* loop over migrated times z */
	for (iz=nz-2; iz>=0; iz--) {
	    /* loop over frequencies w */
	    for (iw=0; iw<nw; iw++) {
		w2 = (eps + I*iw)*dw;
		pp[iw] = q[iz] + pp[iw]*
		    pshift(w2,r,vt[iz],vt[iz+1])*
		    pshift(w2,s,vt[iz],vt[iz+1]);
	    }
	}

	kiss_fftri(invs,(const kiss_fft_cpx *) pp, p);
    } else { /* migration */
	kiss_fftr(forw, p, (kiss_fft_cpx *) pp);

	/* loop over migrated times z */
	for (iz=0; iz<nz-1; iz++) {
	    /* loop over frequencies w */
	    for (iw=0; iw<nw; iw++) {
		w2 = (eps+I*iw)*dw;

		/* accumulate image (summed over frequency) */
		q[iz] += crealf(pp[iw]);
		pp[iw] *= conjf(pshift(w2,r,vt[iz],vt[iz+1])*
				pshift(w2,s,vt[iz],vt[iz+1]));
	    }
	}

	for (iw=0; iw<nw; iw++) {
	    q[nz-1] += crealf(pp[iw]);
	}
    }
}

/* 	$Id$	 */

