#include<stdio.h>
#include<Windows.h>

#define BITS 8
#define FIX(f) ((int)((f) * (1 << BITS) + 0.5))

#define Y_R   FIX(0.257)
#define Y_G   FIX(0.504)
#define Y_B   FIX(0.098)
#define Y_ADD 16

#define U_R   FIX(0.148)
#define U_G   FIX(0.291)
#define U_B   FIX(0.439)
#define U_ADD 128

#define V_R   FIX(0.439)
#define V_G   FIX(0.368)
#define V_B   FIX(0.071)
#define V_ADD 128

void BGR2YV12(unsigned char *dest, unsigned char *src_ori,
	int i_width, int i_height, int stride)
{
	unsigned char *src = src_ori + 3 * i_width*(i_height - 1);
	int     i_src = i_width * 3;
	int     i_y = stride;
	unsigned char *y = dest;
	unsigned char *u = y + stride*i_height;
	unsigned char *v = u + stride*i_height / 4;

	for (; i_height > 0; i_height -= 2)
	{
		unsigned char *ss = src;
		unsigned char *yy = y;
		unsigned char *uu = u;
		unsigned char *vv = v;
		int w;

		for (w = i_width; w > 0; w -= 2)
		{
			int cr = 0, cg = 0, cb = 0;
			int r, g, b;

			/* Luma */
			cr = r = ss[2];
			cg = g = ss[1];
			cb = b = ss[0];

			yy[0] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);

			cr += r = ss[2 - i_src];
			cg += g = ss[1 - i_src];
			cb += b = ss[0 - i_src];
			yy[i_y] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);
			yy++;
			ss += 3;

			cr += r = ss[2];
			cg += g = ss[1];
			cb += b = ss[0];

			yy[0] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);

			cr += r = ss[2 - i_src];
			cg += g = ss[1 - i_src];
			cb += b = ss[0 - i_src];
			yy[i_y] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);
			yy++;
			ss += 3;

			/* Chroma */
			*uu++ = (unsigned char)(U_ADD + ((-U_R * cr - U_G * cg + U_B * cb) >> (BITS + 2)));
			*vv++ = (unsigned char)(V_ADD + ((V_R * cr - V_G * cg - V_B * cb) >> (BITS + 2)));

		}

		src -= 2 * i_src;
		y += 2 * stride;
		u += stride /2;
		v += stride / 2;
	}
}
void BGR2NV12(unsigned char *dest, unsigned char *src_ori,
	int i_width, int i_height, int stride)
{
	unsigned char *src = src_ori + 3 * i_width*(i_height - 1);
	int     i_src = i_width * 3;
	int     i_y = stride;
	unsigned char *y = dest;
	unsigned char *u = y + stride*i_height;
	unsigned char *v = u + 1;

	for (; i_height > 0; i_height -= 2)
	{
		unsigned char *ss = src;
		unsigned char *yy = y;
		unsigned char *uu = u;
		unsigned char *vv = v;
		int w;

		for (w = i_width; w > 0; w -= 2)
		{
			int cr = 0, cg = 0, cb = 0;
			int r, g, b;

			/* Luma */
			cr = r = ss[2];
			cg = g = ss[1];
			cb = b = ss[0];

			yy[0] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);

			cr += r = ss[2 - i_src];
			cg += g = ss[1 - i_src];
			cb += b = ss[0 - i_src];
			yy[i_y] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);
			yy++;
			ss += 3;

			cr += r = ss[2];
			cg += g = ss[1];
			cb += b = ss[0];

			yy[0] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);

			cr += r = ss[2 - i_src];
			cg += g = ss[1 - i_src];
			cb += b = ss[0 - i_src];
			yy[i_y] = Y_ADD + ((Y_R * r + Y_G * g + Y_B * b) >> BITS);
			yy++;
			ss += 3;

			/* Chroma */
			*uu = (unsigned char)(U_ADD + ((-U_R * cr - U_G * cg + U_B * cb) >> (BITS + 2)));
			*vv = (unsigned char)(V_ADD + ((V_R * cr - V_G * cg - V_B * cb) >> (BITS + 2)));

			uu += 2;
			vv += 2;
		}

		src -= 2 * i_src;
		y += 2 * stride;
		u += stride;
		v += stride;
	}
}
#define HEIGHT 240
#define WIDTH  320
int main(int argc, char *argv[])
{
	FILE *pSrc = nullptr;
	FILE *pDest = nullptr;

	errno_t er = fopen_s(&pSrc, "C:\\quanwei\\WMV\\q_W320_H240_F33.bgr", "rb+");
	if (er != 0)
	{
		printf("fopen_s Src failed\n");
	}

	er = fopen_s(&pDest, "C:\\quanwei\\WMV\\TestW.yuv", "wb+");
	if (er != 0)
	{
		printf("fopen_s Dest failed\n");
	}
	
	int iLen = HEIGHT * WIDTH * 3;
	int iFrames = 0;

	unsigned char *buf = new unsigned char[iLen];
	unsigned char *Yuv = new unsigned char[iLen / 2];

	memset(Yuv,0,iLen /2 );
	while (fread(buf,1,iLen,pSrc)!=0)
	{
		BGR2NV12(Yuv, buf, WIDTH, HEIGHT, WIDTH);
		//BGR2YV12(Yuv,buf,WIDTH,HEIGHT,WIDTH);
		
		fwrite(Yuv, 1, iLen / 2, pDest);

		printf("Frames=%d.\n", iFrames++);
		iFrames++;
	}
	
	fclose(pSrc);
	fclose(pDest);
	
	delete[] buf;
	delete[] Yuv;

	return 0;
}