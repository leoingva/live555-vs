

#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH

#include <FramedSource.hh>

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>  // Video write

#define X264_API_IMPORTS
#include"x264.h"

using namespace cv;

class H264FramedLiveSource : public FramedSource
{
public:
	static H264FramedLiveSource* createNew(UsageEnvironment& env,
		char const* fileName,
		unsigned preferredFrameSize = 0,
		unsigned playTimePerFrame = 0);

protected:
	H264FramedLiveSource(UsageEnvironment& env,
		char const* fileName,
		unsigned preferredFrameSize,
		unsigned playTimePerFrame);
	// called only by createNew()
	~H264FramedLiveSource();

private:
	// redefined virtual functions:
	virtual void doGetNextFrame();
	int TransportData(unsigned char* to, unsigned maxSize);

	VideoCapture vcap;
	Mat yuv, frame;
	int y_size;
	x264_t* encoder;
	x264_picture_t pic_in, pic_out;
	x264_nal_t* nals;
	bool initEncoder();
	bool encode();

protected:
	FILE *fp;
};

#endif