#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
//转换矩阵
#define MY(a,b,c) (( a*  0.2989  + b*  0.5866  + c*  0.1145))
#define MU(a,b,c) (( a*(-0.1688) + b*(-0.3312) + c*  0.5000 + 128))
#define MV(a,b,c) (( a*  0.5000  + b*(-0.4184) + c*(-0.0816) + 128))
//#define MY(a,b,c) (( a*  0.257 + b*  0.504  + c*  0.098+16))
//#define MU(a,b,c) (( a*( -0.148) + b*(- 0.291) + c* 0.439 + 128))
//#define MV(a,b,c) (( a*  0.439  + b*(- 0.368) + c*( - 0.071) + 128))

//大小判断
#define DY(a,b,c) (MY(a,b,c) > 255 ? 255 : (MY(a,b,c) < 0 ? 0 : MY(a,b,c)))
#define DU(a,b,c) (MU(a,b,c) > 255 ? 255 : (MU(a,b,c) < 0 ? 0 : MU(a,b,c)))
#define DV(a,b,c) (MV(a,b,c) > 255 ? 255 : (MV(a,b,c) < 0 ? 0 : MV(a,b,c)))
#define CLIP(a) ((a) > 255 ? 255 : ((a) < 0 ? 0 : (a)))
//RGB to YUV
void Convert(unsigned char *RGB, unsigned char *YUV,unsigned int width,unsigned int height)
{
    //变量声明
    unsigned int i,x,y,j;
    unsigned char *Y = NULL;
    unsigned char *U = NULL;
    unsigned char *V = NULL;
    
    Y = YUV;
    U = YUV + width*height;
    V = U + ((width*height)>>2);
    for(y=0; y < height; y++)
        for(x=0; x < width; x++)
        {
            j = y*width + x;
            i = j*3;
            Y[j] = (unsigned char)(DY(RGB[i], RGB[i+1], RGB[i+2]));
            if(x%2 == 1 && y%2 == 1)
            {
                j = (width>>1) * (y>>1) + (x>>1);
                //上面i仍有效
                U[j] = (unsigned char)
                       ((DU(RGB[i  ], RGB[i+1], RGB[i+2]) + 
                         DU(RGB[i-3], RGB[i-2], RGB[i-1]) +
                         DU(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
                         DU(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);
                V[j] = (unsigned char)
                       ((DV(RGB[i  ], RGB[i+1], RGB[i+2]) + 
                         DV(RGB[i-3], RGB[i-2], RGB[i-1]) +
                         DV(RGB[i  -width*3], RGB[i+1-width*3], RGB[i+2-width*3]) +
                         DV(RGB[i-3-width*3], RGB[i-2-width*3], RGB[i-1-width*3]))/4);
            }
        }
}


//转换矩阵
double YuvToRgb[3][3] = {1,       0,  1.4022,
                         1,    -0.3456, -0.7145,
                         1,   1.771,       0};
void YUV2BGR(unsigned char *BGR, unsigned char *YUV,unsigned int width,unsigned int height)
{
    //变量声明
    unsigned int i,x,y,j;
    unsigned char *Y = NULL;
    unsigned char *U = NULL;
    unsigned char *V = NULL;
     int temp = 0;
    Y = YUV;
    U = YUV + width*height;
    V = U + ((width*height)>>2);
   // for(y=0; y < height; y++)
   //     for(x=0; x < width; x++)
   //     {
			//BGR[(y*width+x)*3+2] =(unsigned char)CLIP( 1.164*(Y[(y*width+x)]-16) + 1.596*(V[(y*width/2+x/2)]-128)  );
			//BGR[(y*width+x)*3+1] =(unsigned char)CLIP(  1.164*(Y[(y*width+x)]-16) - 0.813*(V[(y*width/2+x/2)]-128) - 0.392*(U[(y*width/2+x/2)]-128)  );
			//BGR[(y*width+x)*3+0] =(unsigned char)CLIP(  1.164*(Y[(y*width+x)]-16) + 2.017*(U[(y*width/2+x/2)]-128)  );
   //     }
    for(y=0; y < height; y++)
        for(x=0; x < width; x++)
        {
            //r分量
            temp = Y[y*width+x] + (V[(y/2)*(width/2)+x/2]-128) * YuvToRgb[0][2];
            BGR[(y*width+x)*3+2] = temp<0 ? 0 : (temp>255 ? 255 : temp);
            //g分量
            temp = Y[y*width+x] + (U[(y/2)*(width/2)+x/2]-128) * YuvToRgb[1][1]
                                       + (V[(y/2)*(width/2)+x/2]-128) * YuvToRgb[1][2];
            BGR[(y*width+x)*3+1] = temp<0 ? 0 : (temp>255 ? 255 : temp);
            //b分量
            temp = Y[y*width+x] + (U[(y/2)*(width/2)+x/2]-128) * YuvToRgb[2][1];
            BGR[(y*width+x)*3+0] = temp<0 ? 0 : (temp>255 ? 255 : temp);
        }
}

void mergeleftrigth( unsigned char *cut_frame1, unsigned char *ori_frame1,unsigned char *ori_frame2,unsigned int width,unsigned int height)
{
	unsigned int j,i;
	unsigned char *temp_frame1,*temp_frame2;
    temp_frame1 = (unsigned char  *)malloc((height * width + height * width/2)/2 * sizeof(unsigned char ));
	temp_frame2 = (unsigned char  *)malloc((height * width + height * width/2)/2 * sizeof(unsigned char ));
//luma abstract in vertical
			for (j = 0; j < height ; j++)
			{
				for(i=0; i < width; i++)
				{
					if(i % 2 == 0)
						*(temp_frame1+i/2+width/2*j) = *(ori_frame1+i+width*j);
					    *(temp_frame2+i/2+width/2*j) = *(ori_frame2+i+width*j);
				}
			}

			
			//chorma U abstract in vertical
			for (j = 0; j < height/2 ; j++)
			{
				for(i=0; i < width/2; i++)
				{
					if(i % 2 == 0)
						*(temp_frame1+width  * height/2+i/2+width/4*j) = *(ori_frame1+width  * height+i+width/2*j);
					    *(temp_frame2+width  * height/2+i/2+width/4*j) = *(ori_frame2+width  * height+i+width/2*j);
				}
			}
			//chorma V abstract in vertical
			for (j = 0; j < height/2 ; j++)
			{
				for(i=0; i < width/2; i++)
				{
					if(i % 2 == 0)
						*(temp_frame1+width  * height/2+width  * height/8+i/2+width/4*j) = *(ori_frame1+width  * height + width  * height/4 + i+width/2*j);
					    *(temp_frame2+width  * height/2+width  * height/8+i/2+width/4*j) = *(ori_frame2+width  * height + width  * height/4 + i+width/2*j);
				}
			}

        //merge 
			//Y
		for (j = 0; j < height ; j++)
		{
			for(i=0; i < width/2; i++)
				*(cut_frame1+i+j*width)=*(temp_frame1+i + width/2  * j);		
		}

		for (j = 0; j < height ; j++)
		{
			for(i=0; i < width/2; i++)
				*(cut_frame1+width /2+i+j*width)=*(temp_frame2+i + width/2  * j);		
		}

		//U

		for (j = 0; j < height/2 ; j++)
		{
			for(i=0; i < width/4; i++)
				*(cut_frame1+width  * height+i+j*width/2)=*(temp_frame1+width  * height/2+ i + width/4  * j);	
		}
		
		for (j = 0; j < height/2 ; j++)
		{
			for(i=0; i < width/4; i++)
				*(cut_frame1+width  * height+width /4+i+j*width/2)=*(temp_frame2+width  * height/2+ i + width/4  * j);	
		}
		//V


		for (j = 0; j < height/2 ; j++)
		{
			for(i=0; i < width/4; i++)
				*(cut_frame1+width  * height+width  * height/4+i+j*width/2)=*(temp_frame1+width  * height/2+ width  * height/8+i + width/4  * j);	
		}
		
		for (j = 0; j < height/2 ; j++)
		{
			for(i=0; i < width/4; i++)
				*(cut_frame1+width  * height+width  * height/4+ width /4+i+j*width/2)=*(temp_frame2+width  * height/2+ width  * height/8+i + width/4  * j);	
		}
		free(temp_frame1);
		free(temp_frame2);
}