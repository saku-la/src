#include <stdio.h>
#include <math.h>
#include <string.h>

#include <rsfplot.h>

#define MAXLINE 2016

static void text (void);

int main (int argc, char* argv[])
{
    int npat, line_count, ix, iy, iz, npts, mtype, orient, maskx, masky;
    int i, j, nmul, ipat, col, ras_orient, ras_offset, col_tab_no, nx, ny;
    int xpix, ypix, num_rep, count, num_pat, num_byte, byte, ibyte=0;
    float fatscale, txvecscale, txscale, scale, colscale, hscale;
    float x, y, xcor, ycor, msize, size, fat, off, rep, red, green, blue;
    float xmin, ymin, xmax, ymax, xvplot, yvplot;
    char line[MAXLINE], c, a, *ptr;

    vp_init();

    fatscale = 1.;
    txvecscale = 1.;
    txscale = 1.;
    scale = 1.;
    colscale = 1.;
    hscale = 1.;

    npat=0;

    for (line_count=0; fgets (line, MAXLINE, stdin) != NULL; line_count++) {
	c = line[0];

	switch (c) {
	    case '#':
		if (0 == line_count &&
		    0 == strncmp (line, "#plas: ", 7)) {
		    switch (line[7]) {
			case 'V':
			case 'v':
			    fatscale = 1.;
			    txvecscale = 1.;
			    txscale = 1.;
			    scale = 1.;
			    colscale = 1.;
			    hscale = 1;
			    break;
			case 'I':
			case 'i':
			    fatscale = FATPERIN;
			    txvecscale = TEXTVECSCALE;
			    txscale = TXPERIN;
			    scale = RPERIN;
			    colscale = MAX_GUN;
			    hscale = RPERIN;
			    break;
			case 'C':
			case 'c':
			    fatscale = FATPERIN / 2.54;
			    txvecscale = TEXTVECSCALE;
			    txscale = TXPERIN / 2.54;
			    scale = RPERIN / 2.54;
			    colscale = MAX_GUN;
			    hscale = RPERIN / 2.54;
			    break;
		    }
		}
		break;
	    case VP_MESSAGE:
		putchar (c);
		text ();
		break;
	    case VP_SETSTYLE:
		sscanf (line, "%*c %c", &a);
		putchar (c);
		putchar (a);
		break;
	    case VP_ERASE:
	    case VP_PURGE:
	    case VP_BREAK:
	    case VP_NOOP:
		putchar (c);
		break;
	    case VP_TXALIGN:
		putchar (c);
		sscanf (line, "%*c %d %d", &ix, &iy);
		vp_putint (ix);
		vp_putint (iy);
		break;
	    case VP_ORIGIN:
	    case VP_MOVE:
	    case VP_DRAW:
		putchar (c);
		sscanf (line, "%*c %f %f", &x, &y);
		vp_putfloat0 (x * scale);
		vp_putfloat0 (y * scale);
		break;
	    case VP_TXFONTPREC:
		putchar (c);
		sscanf (line, "%*c %d %d %d", &ix, &iy, &iz);
		vp_putint (ix);
		vp_putint (iy);
		vp_putint (iz);
		break;
	    case VP_PLINE:
	    case VP_SETDASH:
		putchar (c);
		sscanf (line, "%*c %d", &npts);
		vp_putint (npts);
		while (npts--) {
		    fgets (line, MAXLINE, stdin);
		    sscanf (line, "%f %f", &x, &y);
		    vp_putfloat0 (x * scale);
		    vp_putfloat0 (y * scale);
		}
		break;
	    case VP_PMARK:
		putchar (c);
		sscanf (line, "%*c %d %d %f", &npts, &mtype, &msize);
		vp_putint (npts);
		vp_putint (mtype);
		vp_putfloat0 (msize * txscale);
		while (npts--) {
		    fgets (line, MAXLINE, stdin);
		    sscanf (line, "%f %f", &x, &y);
		    vp_putfloat0 (x * scale);
		    vp_putfloat0 (y * scale);
		}
		break;
	    case VP_BEGIN_GROUP:
		putchar (c);
		text ();
		break;
	    case VP_END_GROUP:
		putchar (c);
		break;
	    case VP_OLDTEXT:
	    case VP_TEXT:
		putchar (VP_TEXT);
		sscanf (line, "%*c %f %d", &size, &orient);
		vp_putfloat0 (size * txscale);
		if (VP_OLDTEXT == c) orient *= 90;
		vp_putint (orient);
		text ();
		break;
	    case VP_GTEXT:
		putchar (c);
		sscanf (line, "%*c %f %f %f %f", &x, &y, &xcor, &ycor);
		vp_putfloat0 (x * scale * txvecscale);
		vp_putfloat0 (y * scale * txvecscale);
		vp_putfloat0 (xcor * scale * txvecscale);
		vp_putfloat0 (ycor * scale * txvecscale);
		text ();
		break;
	    case VP_COLOR:
	    case VP_OVERLAY:
		putchar (c);
		sscanf (line, "%*c %d", &ix);
		vp_putint (ix);
		break;
	    case VP_SET_COLOR_TABLE:
		putchar (c);
		sscanf (line, "%*c %d %f %f %f",
			&col_tab_no, &red, &green, &blue);
		vp_putint (col_tab_no);
		vp_putfloat0 (red * colscale);
		vp_putfloat0 (green * colscale);
		vp_putfloat0 (blue * colscale);
		break;
	    case VP_FAT:
		putchar (c);
		sscanf (line, "%*c %f", &fat);
		vp_putfloat0 (fat * fatscale);
		break;
	    case VP_WINDOW:
		putchar (c);
		sscanf (line, "%*c %f %f %f %f", &xmin, &ymin, &xmax, &ymax);
		vp_putfloat0 (scale * xmin);
		vp_putfloat0 (scale * ymin);
		vp_putfloat0 (scale * xmax);
		vp_putfloat0 (scale * ymax);
		break;
	    case VP_OLDAREA:
		putchar (c);
		sscanf (line, "%*c %d", &npts);
		fgets (line, MAXLINE, stdin);
		sscanf (line, "%f %d %d", &fat, &maskx, &masky);
		vp_putint (npts);
		vp_putfloat0 (fat * fatscale);
		vp_putint (maskx);
		vp_putint (masky);
		for (i = 0; i < npts; i++) {
		    fgets (line, MAXLINE, stdin);
		    sscanf (line, "%f %f", &x, &y);
		    vp_putfloat0 (x * scale);
		    vp_putfloat0 (y * scale);
		}
		break;
	    case VP_AREA:
		putchar (c);
		sscanf (line, "%*c %d", &npts);
		vp_putint (npts);
		for (i = 0; i < npts; i++) {
		    fgets (line, MAXLINE, stdin);
		    sscanf (line, "%f %f", &x, &y);
		    vp_putfloat0 (scale * x);
		    vp_putfloat0 (scale * y);
		}
		break;
	    case VP_PATLOAD:
		npat++;
		ipat = 0;
		sscanf (line, "%*c %d %d %d %d", &nmul, &nx, &ny, &ipat);
		putchar (c);
		vp_putint (nmul);
		vp_putint (nx);
		vp_putint (ny);
		if (0 == ipat) ipat = npat;
		vp_putint (ipat);
		if (-1 != nx) {
		    for (i = 0; i < nx; i++) {
			fgets (line, MAXLINE, stdin);
			for (j = 0, ptr = line; j < ny; j++, ptr++) {
			    if ('\0' == *ptr || '\n' == *ptr)
				fprintf (stderr, "null/nl");
			    if (*ptr >= '0' && *ptr <= '9') {
				ipat = (*ptr) - '0';
			    } else {
				ipat = (*ptr) - 'A' + 10;
			    }
			    vp_putint (ipat);
			}
		    }
		} else {
		    for (i = 0; i < ny * 2; i++) {
			fgets (line, MAXLINE, stdin);
			sscanf (line, "%f %d %f %f", &fat, &col, &off, &rep);
			vp_putfloat0 (fat * fatscale);
			vp_putint (col);
			vp_putfloat0 (off * hscale);
			vp_putfloat0 (rep * hscale);
		    }
		} break;
	    case VP_BYTE_RASTER:
	    case VP_BIT_RASTER:
		sscanf (line, "%*c %d %d", &ras_orient, &ras_offset);
		putchar (c);
		vp_putint (ras_orient);
		vp_putint (ras_offset);
		fgets (line, MAXLINE, stdin);
		sscanf (line, "%f %f", &xcor, &ycor);
		vp_putfloat0 (xcor * scale);
		vp_putfloat0 (ycor * scale);
		fgets (line, MAXLINE, stdin);
		sscanf (line, "%f %f", &xvplot, &yvplot);
		vp_putfloat0 (scale * xvplot);
		vp_putfloat0 (scale * yvplot);
		fgets (line, MAXLINE, stdin);
		sscanf (line, "%d %d", &xpix, &ypix);
		vp_putint (xpix);
		vp_putint (ypix);

		for (j = 0; j < ypix; j += num_rep) {
		    fgets (line, MAXLINE, stdin);
		    sscanf (line, "%d", &num_rep);
		    if (num_rep < 1)
			fprintf (stderr, "Bad Raster repetition factor\n");
		    vp_putint (num_rep);
		    for (count=0; count < xpix; count += num_byte * num_pat) {
			fgets (line, MAXLINE, stdin);
			sscanf (line, "%d %d", &num_pat, &num_byte);
			vp_putint (num_pat);
			vp_putint (num_byte);
			for (i = 0; i < num_byte; i++) {
			    fgets (line, MAXLINE, stdin);
			    sscanf (line, "%d", &byte);
			    if (VP_BYTE_RASTER == c) {
				putchar ((char) byte);
			    } else {
				if (i % 8 == 0)
				    ibyte = 0;
				ibyte |= ((byte != 0) << (7 - (i % 8)));
				if (i % 8 == 7 || i == num_byte - 1)
				    putchar ((char) ibyte);
			    }
			}
		    }
		}
		break;
	    default:
		break;  /* Treat unknown charactors as comments */
	}
    }

    return 0;
}

static void text (void)
{
    char c;
    
    while ('\n' != (c = getchar ())) {
	if ('\\' != c || '\n' != getchar ()) 
	    putchar (c);
    }
    putchar ('\0');
}

