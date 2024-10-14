/*
 * Example made with love by Natxopedreira 2021
 * https://github.com/natxopedreira
 * Updated by members of the ZKM | Hertz-Lab 2021
 */

#pragma once

#include "ofMain.h"
#include "ofxTensorFlow2.h"
#include "ofxOpenCv.h"
#include "ofxSyphon.h"

// uncomment this to use a live camera, otherwise we'll use a video file
//#define USE_LIVE_VIDEO

class ofApp : public ofBaseApp {

	public:
    const static int numFramesToGather=60;
    struct bestFit{
        std::vector<cv::Point> charContours;
        std::vector<cv::Point> bodyContours;
        float sim = 10.;
        float rotation;
        char letter;
        ofPixels videoFrames[numFramesToGather];
        float centerX = 0;
        float centerY = 0;
//        std::vector<ofImage> videoFrames;
    };

    
		void setup();
		void update();
		void draw();
    ofImage     img;
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        ofxSyphonClient mClient;
    ofxSyphonServerDirectory dir;
    ofPixels pixels;
    
    ofImage syphonImage;
    ofPixels syphonPixels;
//    ofxSyphonClient client;
    int dirIdx;
		// neural net input size
		float nnWidth = 1920;
		float nnHeight = 1080;

		// input
    int camWidth = 1920;
    int camHeight = 1080;
    ofImage streamImage;
    int letterPlace;
//		#ifdef USE_LIVE_VIDEO

//			ofVideoGrabber video;
//		#else
			ofVideoPlayer video;
//		#endif

		// model
		ofxTF2::Model model;

		std::vector<cppflow::tensor> inputs;
    const static int numChars =63;
        bestFit bestFits[numChars];
		ofImage imgMask;
		ofImage imgBackground;
		ofImage imgOut;
        ofImage saveImgs[numFramesToGather];
        
        
        ofPixels curVideoFrames[numFramesToGather];
    
        int gatherFrames=0;
    int frameIndex = 0;
        ofFbo fbo;
        ofFloatPixels pix;
    
        ofxCvGrayscaleImage     grayImage;
        ofxCvGrayscaleImage characterImage;
        ofTrueTypeFont testFont;
        ofTrueTypeFont testFont2;
    float rotation;
    
    float centerX ;
    float centerY ;
    
    float centerXTemp ;
    float centerYTemp ;
    
    ofMatrix4x4 homoMat;
        ofPath testChar, testCharContour;
    
        uint32_t letter;
        ofxCvContourFinder     contourFinder;
    float sim = 10.;
    char capitalLetters[26];


};
