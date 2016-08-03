
#ifndef _ENCODEAPP_H_
#define _ENCODEAPP_H_

#include "x264.h"
#include <stdint.h>
#define WRITEOUT_RECONSTRUCTION 1

typedef int32_t INT32;
typedef  signed char INT8;


typedef struct _EncodeApp 
{
    x264_t *h;
    x264_picture_t pic;
    x264_param_t param;

	void *outBufs;//一帧码流的缓存
	int  outBufslength;//总大小
	int   bitslen;//实际码流大小

	FILE *bits;                     // point to output bitstream file 
#ifdef WRITEOUT_RECONSTRUCTION	
	FILE *p_rec ;
#endif
}EncodeApp;
INT32 EncoderInit(EncodeApp*  EncApp,int width,int height );
INT32 EncoderEncodeFrame(EncodeApp*  EncApp);
INT32 EncoderDestroy(EncodeApp*  EncApp);

#endif