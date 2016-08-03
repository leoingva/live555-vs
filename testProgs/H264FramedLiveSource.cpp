

#include "H264FramedLiveSource.hh"
#include <iostream>
#include <string>

#define X264_API_IMPORTS
#include"x264.h"
#define readfromfile 0

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment& env,
	char const* fileName,
	unsigned preferredFrameSize,
	unsigned playTimePerFrame)
	: FramedSource(env)
{
#if readfromfile
	fp = fopen(fileName, "rb");
#else
	vcap.open(0);
	//vcap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('1', '2', '3', '4'));
	//vcap.set(CV_CAP_PROP_FPS, 15);
	//vcap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	//vcap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
	//vcap.set(CV_CAP_PROP_BUFFERSIZE, 32768);
	//vcap.set(CV_CAP_PROP_CONVERT_RGB, 1);
	if (!vcap.isOpened()) {
		std::cout << "camera open failed";
		return ;
	}

	if (initEncoder() == false) {
		std::cout << "encoder init failed";
		return;
	}
#endif
}

H264FramedLiveSource* H264FramedLiveSource::createNew(UsageEnvironment& env,
	char const* fileName,
	unsigned preferredFrameSize /*= 0*/,
	unsigned playTimePerFrame /*= 0*/)
{
	H264FramedLiveSource* newSource = new H264FramedLiveSource(env, fileName, preferredFrameSize, playTimePerFrame);

	return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
#if readfromfile
	fclose(fp);
#else
	vcap.release();
	x264_encoder_close(encoder);
	frame.refcount = 0;
	frame.release();
	yuv.refcount = 0;
	yuv.release();
#endif
}


long filesize(FILE *stream)
{
	long curpos, length;
	curpos = ftell(stream);
	fseek(stream, 0L, SEEK_END);
	length = ftell(stream);
	fseek(stream, curpos, SEEK_SET);
	return length;
}

void H264FramedLiveSource::doGetNextFrame()
{
#if readfromfile
	if (filesize(fp) >  fMaxSize)
		fFrameSize = fread(fTo, 1, fMaxSize, fp);
	else
	{
		fFrameSize = fread(fTo, 1, filesize(fp), fp);
		fseek(fp, 0, SEEK_SET);
	}
	fFrameSize = fMaxSize;
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,//delay 0 sec and then run aftergetting
		(TaskFunc*)FramedSource::afterGetting, this);
	return;
#else
	

	//if (!isCurrentlyAwaitingData()) return;
	//

	if (encode() == false) return;

	//unsigned newFrameSize = nals->i_payload;

	// Deliver the data here:
	//if (newFrameSize > fMaxSize) {
	//	fFrameSize = fMaxSize;
	//	fNumTruncatedBytes = newFrameSize - fMaxSize;
	//}
	//else {
	//	fFrameSize = newFrameSize;
	//}





	//memcpy(fTo, nals->p_payload, nals->i_payload);

	nextTask() = envir().taskScheduler().scheduleDelayedTask(0, (TaskFunc*)FramedSource::afterGetting, this);//
	return;
#endif
}


#define FPS 15
bool H264FramedLiveSource::initEncoder()
{
	x264_param_t param;
	x264_param_default_preset(&param, "veryfast", "zerolatency");
	param.i_threads = 1;
	param.i_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	param.i_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	param.i_fps_num = FPS;
	param.i_fps_den = 1;

	param.i_keyint_max = 15;
	param.b_intra_refresh = 1;

	param.rc.i_rc_method = X264_RC_CRF;
	param.rc.f_rf_constant = 25;
	param.rc.f_rf_constant_max = 35;

	param.b_repeat_headers = 1;
	param.b_annexb = 1;
	x264_param_apply_profile(&param, "baseline");

	encoder = x264_encoder_open(&param);
	if (!encoder)
		return false;
	x264_picture_alloc(&pic_in, X264_CSP_I420, param.i_width, param.i_height);
	
	y_size = param.i_width * param.i_height;

	return true;

}

#if defined(_WINDOWS_)
#pragma comment (lib, "Winmm.lib")
#endif

bool H264FramedLiveSource::encode()
{
	
	vcap >> frame;
	cv::cvtColor(frame, yuv, CV_BGR2YUV_I420);
	memcpy(pic_in.img.plane[0], yuv.data, y_size);
	memcpy(pic_in.img.plane[1], yuv.data + y_size, y_size / 4);
	memcpy(pic_in.img.plane[2], yuv.data + (y_size * 5 / 4), y_size / 4);
	//if (waitKey(30) >= 0) return 0;

	int i_nals;
	int frame_size = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);
	if (frame_size <= 0) return false;
	
	fFrameSize = 0;
	for (int j = 0; j < i_nals; ++j) {
		//if (fFrameSize + nals[j].i_payload > fMaxSize)
		//{
		//	printf("fMaxSize!!!!!!!!!!!!\n");
		//	break;
		//}
		memcpy((unsigned char*)fTo + fFrameSize, nals[j].p_payload, nals[j].i_payload);
		fFrameSize += nals[j].i_payload;
	}

#if defined(_WINDOWS_)
	timeval currentTime;
	DWORD t = timeGetTime();
	currentTime.tv_sec = t / 1000;
	currentTime.tv_usec = t % 1000;
	fPresentationTime = currentTime;
#else
	fPresentationTime = gettimeofday(&currentTime, NULL);
#endif
	return  true;

}