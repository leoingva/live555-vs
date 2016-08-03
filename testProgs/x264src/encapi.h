/*
*****************************************************************************************
* \Brief :  Define the 4 API function proto type 
* \Author:  Bin Feng <frobby@163.com>
* \Date  :  2007-09-25
*****************************************************************************************
*/
#ifndef _ENCAPI_H_
#define _ENCAPI_H_

#include "..\inc\enclibvaris.h"

XDAS_Int32 VIDENCAVS_WNLO_initObj(
	VIDENCAVS_WNLO_Obj *handle,				//编码器的句柄
	const IALG_MemRec memTab[], 		    //not care
	VIDENCAVS_WNLO_Obj p,					//not care
	const IVIDENC_Params *algParams);		//输入参数，定义见IVIDENC_Params

XDAS_Int32 VIDENCAVS_WNLO_process(
	VIDENCAVS_WNLO_Obj *h, 		            //编码器的句柄
	XDM_BufDesc *inBufs,			    	//输入buffer，定义见XDM_BufDesc，
                                            //我们用该inBufs-> bufs[0] 用于存放待编码的原始数据
                                            //inBufs->numBufs=1
                                            //inBufs-> bufSizes[0]=输入原始数据的长度	
    XDM_BufDesc *outBufs, 			        //输出buffer，定义见XDM_BufDesc用于存放编码后的码流
                                            //我们用该outBufs -> bufs[0] 用于存放编码后的码流
                                            // outBufs ->numBufs=1
                                            // outBufs -> bufSizes[0]=编码后码流的长度	
	IVIDENC_InArgs *inArgs, 			    //编码过程输入参数，定义见IVIDENC_InArgs
	IVIDENC_OutArgs *outArgs);		        //编码过程输出参数，定义见IVIDENC_OutArgs

XDAS_Int32 VIDENCAVS_WNLO_control(
	VIDENCAVS_WNLO_Obj *handle, 			    //编码器的句柄
	IVIDENC_Cmd id,							//命令
	IVIDENC_DynamicParams *params, 			//动态修改的参数，定义见IVIDENC_DynamicParams
	IVIDENC_Status *status);				//返回编码器的状态，定义见IVIDENC_Status

XDAS_Int32 VIDENCAVS_WNLO_free(
    VIDENCAVS_WNLO_Obj *handle,			    //编码器的句柄
    IALG_MemRec memTab[]);				    //not care





#endif