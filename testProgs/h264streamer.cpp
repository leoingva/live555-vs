
#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264FramedLiveSource.hh"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#pragma comment (lib, "Ws2_32.lib")  
#pragma comment (lib, "BasicUsageEnvironment.lib")
#pragma comment (lib, "groupsock.lib")
#pragma comment (lib, "liveMedia.lib")
#pragma comment (lib, "UsageEnvironment.lib")
#pragma comment (lib, "libx264.lib")
UsageEnvironment* env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
	char const* streamName, char const* inputFileName); // fwd

static char newMatroskaDemuxWatchVariable;
static MatroskaFileServerDemux* demux;
static void onMatroskaDemuxCreation(MatroskaFileServerDemux* newDemux, void* /*clientData*/) {
	demux = newDemux;
	newMatroskaDemuxWatchVariable = 1;
}
#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace cv;
using namespace std;
#include <cv.h>
#include <highgui.h>

#define test5 0
#if test5
void main()
{

	// Load input video
	cv::VideoCapture input_cap("d:\\a.mp4");
	if (!input_cap.isOpened())
	{
		std::cout << "!!! Input video could not be opened" << std::endl;
		return;
	}

	// Setup output video
	cv::VideoWriter output_cap("b.mp4",
		input_cap.get(CV_CAP_PROP_FOURCC),
		input_cap.get(CV_CAP_PROP_FPS),
		cv::Size(input_cap.get(CV_CAP_PROP_FRAME_WIDTH),
			input_cap.get(CV_CAP_PROP_FRAME_HEIGHT)));

	if (!output_cap.isOpened())
	{
		std::cout << "!!! Output video could not be opened" << std::endl;
		return;
	}


	// Loop to read from input and write to output
	cv::Mat frame;

	while (true)
	{
		if (!input_cap.read(frame))
			break;

		output_cap.write(frame);
	}

	input_cap.release();
	output_cap.release();
}
#endif

#define test4 0
#if test4
int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	Mat edges;
	Mat frame;
	std::string windowname("wnd");
	//namedWindow(windowname, 1);
	for (;;)
	{
		cap >> frame; // get a new frame from camera
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		imshow(windowname, edges);
		if (waitKey(20) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}
#endif



#define test2 0
#if test2
#include "opencv2/opencv.hpp"
#include <iostream>
#include"x264.h"
int main(int argc, char *argv[])
{
	VideoCapture vcap(0);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);
	if (!video.isOpened())
		return 0;
	Mat frame;
	for (;;) {

		vcap >> frame;
		video.write(frame);
		//imshow("Frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
	return 0;

}
#endif

//struct CvVideoWriter_FFMPEG p;
#define test1 0
#if test1
int main()
{
	IplImage *pFrame = NULL;
	CvCapture *pCapture = cvCreateCameraCapture(0); 
	cvNamedWindow("video", 1);
	pFrame = cvQueryFrame(pCapture);
	cvShowImage("video", pFrame);
	//



	//IplImage * test;
	//test = cvLoadImage("D:\\11.jpg");//
	//cvNamedWindow("test_demo", 1);
	//cvShowImage("test_demo", test);
	cvWaitKey(0);
	///cvDestroyWindow("test_demo");
	//cvReleaseImage(&test);
	cvDestroyWindow("video");
	cvReleaseImage(&pFrame);

	return 0;
}
#endif

#define testencodetofile 0
#if testencodetofile
void main()
{
	int ret;
	int y_size;
	int i, j;
	VideoCapture cam(0);
	FILE* fp_dst = fopen("d:\\x264.264", "wb");
	int frame_num = 0;
	int csp = X264_CSP_I420;
	int width = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = cam.get(CV_CAP_PROP_FRAME_HEIGHT);
	int iNal = 0;
	x264_nal_t* pNals = NULL;
	x264_t* pHandle = NULL;
	x264_picture_t* pPic_in = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	x264_picture_t* pPic_out = (x264_picture_t*)malloc(sizeof(x264_picture_t));
	x264_param_t* pParam = (x264_param_t*)malloc(sizeof(x264_param_t));
	x264_param_default(pParam);
	pParam->i_width = width;
	pParam->i_height = height;
	pParam->i_csp = csp;
	x264_param_apply_profile(pParam, x264_profile_names[5]);
	pHandle = x264_encoder_open(pParam);
	x264_picture_init(pPic_out);
	x264_picture_alloc(pPic_in, csp, pParam->i_width, pParam->i_height);
	y_size = pParam->i_width * pParam->i_height;
	cv::Mat edges;
	cv::Mat yuv;
	for (i = 0; i <= 200; i++) {
		cam >> edges;
		cv::cvtColor(edges, yuv, COLOR_BGR2YUV_I420);
		memcpy(pPic_in->img.plane[0], yuv.data, y_size);
		memcpy(pPic_in->img.plane[1], yuv.data + y_size, y_size / 4);
		memcpy(pPic_in->img.plane[2], yuv.data + (y_size * 5 / 4 ), y_size / 4);
		pPic_in->i_pts = i;
		ret = x264_encoder_encode(pHandle, &pNals, &iNal, pPic_in, pPic_out);
		if (ret< 0) {
			printf("Error.\n");
			return ;
		}
		printf("Succeed encode frame: %5d\n", i);
		for (j = 0; j < iNal; ++j) {
			fwrite(pNals[j].p_payload, 1, pNals[j].i_payload, fp_dst);
		}
		waitKey(30);
	}
	x264_picture_clean(pPic_in);
	x264_encoder_close(pHandle);
	pHandle = NULL;
	free(pPic_in);
	free(pPic_out);
	free(pParam);
	fclose(fp_dst);
}
#endif


#define teststream264 1
#if teststream264
int main(int argc, char** argv) {
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
													 // Repeat the above with each <username>, <password> that you wish to allow
													 // access to the server.
#endif

													 // Create the RTSP server:
	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	char const* descriptionString
		= "Session streamed by \"testOnDemandRTSPServer\"";

	// Set up each of the possible streams that can be served by the
	// RTSP server.  Each such stream is implemented using a
	// "ServerMediaSession" object, plus one or more
	// "ServerMediaSubsession" objects for each audio/video substream.


	// A H.264 video elementary stream:
	{
		char const* streamName = "cam";
		char const* inputFileName = "test.264";
		ServerMediaSession* sms
			= ServerMediaSession::createNew(*env, streamName, streamName,
				descriptionString);

		sms->addSubsession(H264LiveVideoServerMediaSubssion
			::createNew(*env, inputFileName, reuseFirstSource));// own servermedia  H264LiveVideoServerMediaSubssion
		

		rtspServer->addServerMediaSession(sms);

		announceStream(rtspServer, sms, streamName, inputFileName);
	}



	// Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
	// Try first with the default HTTP port (80), and then with the alternative HTTP
	// port numbers (8000 and 8080).

	if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
	  *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
	} else {
	  *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
	}

	env->taskScheduler().doEventLoop(); // does not return

	return 0; // only to prevent compiler warning
}
#endif
static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
	char const* streamName, char const* inputFileName) {
	char* url = rtspServer->rtspURL(sms);
	UsageEnvironment& env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream, from the file \""
		<< inputFileName << "\"\n";
	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}