#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> //to allow read() function




UsageEnvironment* env;

H264VideoStreamFramer* videoSource;
RTPSink* videoSink;

void play(); // forward
			 //-------------------------------------------------------------------------
			 //Entry Point -> Main FUNCTION  
			 //-------------------------------------------------------------------------

int main(int argc, char** argv) {
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	// Create 'groupsocks' for RTP and RTCP:
	struct in_addr destinationAddress;
	destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
	// Note: This is a multicast address.  If you wish instead to stream
	// using unicast, then you should use the "testOnDemandRTSPServer"
	// test program - not this test program - as a model.

	const unsigned short rtpPortNum = 18888;
	const unsigned short rtcpPortNum = rtpPortNum + 1;
	const unsigned char ttl = 255;

	const Port rtpPort(rtpPortNum);
	const Port rtcpPort(rtcpPortNum);

	Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
	rtpGroupsock.multicastSendOnly(); // we're a SSM source
	Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
	rtcpGroupsock.multicastSendOnly(); // we're a SSM source

									   // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 600000;
	videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

	// Create (and start) a 'RTCP instance' for this RTP sink:
	const unsigned estimatedSessionBandwidth = 1024; // in kbps; for RTCP b/w share
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen + 1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0'; // just in case
	RTCPInstance* rtcp
		= RTCPInstance::createNew(*env, &rtcpGroupsock,
			estimatedSessionBandwidth, CNAME,
			videoSink, NULL /* we're a server */,
			True /* we're a SSM source */);
	// Note: This starts RTCP running automatically

	RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}
	ServerMediaSession* sms
		= ServerMediaSession::createNew(*env, "ipcamera", "UPP Buffer",
			"Session streamed by \"testH264VideoStreamer\"",
			True /*SSM*/);
	sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
	rtspServer->addServerMediaSession(sms);

	char* url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;

	// Start the streaming:
	*env << "Beginning streaming...\n";
	play();

	env->taskScheduler().doEventLoop(); // does not return

	return 0; // only to prevent compiler warning
}
//----------------------------------------------------------------------
//AFTER PLAY FUNCTION CALLED HERE
//----------------------------------------------------------------------
void afterPlaying(void* /*clientData*/)
{

	play();
}
//------------------------------------------------------------------------
//PLAY FUNCTION () 
//------------------------------------------------------------------------
void play()
{


	// Open the input file as with Device as the source:
	DeviceSource* devSource
		= DeviceSource::createNew(*env);
	if (devSource == NULL)
	{

		*env << "Unable to read from\"" << "Buffer"
			<< "\" as a byte-stream source\n";
		exit(1);
	}

	FramedSource* videoES = devSource;

	// Create a framer for the Video Elementary Stream:
	videoSource = H264VideoStreamFramer::createNew(*env, videoES, False);

	// Finally, start playing:
	*env << "Beginning to read from UPP...\n";
	videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}