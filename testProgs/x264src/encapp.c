 #include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "encodeapp.h"

#pragma comment(lib,"libx264.lib")





extern int  Encode_frame(EncodeApp*  EncApp, x264_t *h, x264_picture_t *pic );
INT32 EncoderEncodeFrame(EncodeApp*  EncApp)
{
		//printf("processing frame %d...",j);
		int i;
		//int     i_frame, i_frame_total;
		int64_t i_file=0;

    /* Encode frames */
        //EncApp->pic.i_pts = (int64_t)i_frame * (&EncApp->param)->i_fps_den;
        {
            /* Do not force any parameters */
            EncApp->pic.i_type = X264_TYPE_AUTO;
            EncApp->pic.i_qpplus1 = 0;
        }

		i_file += Encode_frame(EncApp, EncApp->h, &EncApp->pic );
	
		//fwrite(EncApp,EncApp->bits);

#ifdef WRITEOUT_RECONSTRUCTION		
		//write reconstruction
#endif
	return 0;
}

INT32 EncoderDestroy(EncodeApp*  EncApp)
{

     x264_picture_clean( &EncApp->pic );
	x264_encoder_close( EncApp->h );

	if(EncApp->outBufs )	
		free(EncApp->outBufs );	 


	fclose(EncApp->bits);
#ifdef WRITEOUT_RECONSTRUCTION	
	fclose(EncApp->p_rec);
#endif
	return 0;
}


/////////////////////////////
#define X264_API_IMPORTS
#include"x264.h"
