/*
 * Example made with love by Natxopedreira 2021
 * https://github.com/natxopedreira
 * Updated by members of the ZKM | Hertz-Lab 2021
 */

#include "ofApp.h"

void makeDir(std::string dirName){
   

    // Create an ofDirectory object
    ofDirectory dir;

    // Set the path to the new directory
    dir = ofDirectory(ofToDataPath(dirName));

    // Check if the directory already exists
    if(!dir.exists()){
        // Create the directory(s) if it doesn't exist
        bool success = dir.create(true);
        if(success){
            ofLogNotice() << "Directory " << dirName << " created successfully!";
        } else {
            ofLogError() << "Failed to create directory " << dirName;
        }
    } else {
        ofLogNotice() << "Directory " << dirName << " already exists!";
    }
}
void drawPointsOpenFrameworksStyle(const std::vector<cv::Point>& points) {
    // Begin shape
    ofBeginShape();

    // Iterate over each point in the vector and draw it as a vertex
    for (const auto& point : points) {
        ofVertex(point.x, point.y);
    }

    // End shape
    ofEndShape();
}

ofMatrix4x4 cvMatToOFMatrix(const cv::Mat& cvMat) {
    // Create an empty ofMatrix4x4
    ofMatrix4x4 ofMat;
    
    // Check if the input matrix is empty
    if (cvMat.empty()) {
        // Return an empty ofMatrix4x4 if cv::Mat is empty
        return ofMat;
    }
    
    // Ensure the cv::Mat is a 2x3 matrix of type CV_32F
    if (cvMat.rows == 2 && cvMat.cols == 3) {
        // Initialize the 4x4 ofMatrix4x4 with default values
        ofMat.makeIdentityMatrix();

        // Copy values from the 2x3 matrix to the 4x4 matrix
        ofMat(0, 0) = cvMat.at<float>(0, 0);
        ofMat(0, 1) = cvMat.at<float>(0, 1);
        ofMat(0, 3) = cvMat.at<float>(0, 2);

        ofMat(1, 0) = cvMat.at<float>(1, 0);
        ofMat(1, 1) = cvMat.at<float>(1, 1);
        ofMat(1, 3) = cvMat.at<float>(1, 2);

        // The third row is (0, 0, 1) for affine transformations
        ofMat(2, 2) = 1.0f;
        ofMat(3, 3) = 1.0f;
    } else {
        // Throw an exception or handle error if the matrix is not 2x3 or not of type CV_32F
        throw std::invalid_argument("Input cv::Mat must be a 2x3 matrix of type CV_32F.");
    }

    return ofMat;
}
//
//std::vector<cv::Point> resampleContour(const std::vector<cv::Point>& contour, int newSize) {
//    std::vector<cv::Point> resampled;
//    int originalSize = contour.size();
//
//    if (newSize <= 0 || originalSize == 0) {
//        // Return an empty vector if the new size is invalid or the original contour is empty
//        return resampled;
//    }
//
//    if (newSize == originalSize) {
//        // If the new size is the same as the original, return the original contour
//        return contour;
//    }
//
//    // Calculate the cumulative arc length along the contour
//    std::vector<float> arcLength(originalSize);
//    arcLength[0] = 0;
//    for (int i = 1; i < originalSize; ++i) {
//        arcLength[i] = arcLength[i - 1] + cv::norm(contour[i] - contour[i - 1]);
//    }
//    float totalLength = arcLength.back();
//
//    // Generate new equally spaced arc lengths
//    std::vector<float> newArcLengths(newSize);
//    for (int i = 0; i < newSize; ++i) {
//        newArcLengths[i] = (i * totalLength) / (newSize - 1);
//    }
//
//    // Resample the contour
//    resampled.push_back(contour[0]); // First point remains the same
//    int nextPointIndex = 1;
//    for (int i = 1; i < newSize - 1; ++i) {
//        while (newArcLengths[i] > arcLength[nextPointIndex] && nextPointIndex < originalSize - 1) {
//            nextPointIndex++;
//        }
//
//        float t = (newArcLengths[i] - arcLength[nextPointIndex - 1]) / (arcLength[nextPointIndex] - arcLength[nextPointIndex - 1]);
//        cv::Point newPoint = contour[nextPointIndex - 1] * (1 - t) + contour[nextPointIndex] * t;
//        resampled.push_back(newPoint);
//    }
//    resampled.push_back(contour.back()); // Last point remains the same
//
//    return resampled;
//}



std::vector<cv::Point> resampleContour(const std::vector<cv::Point>& contour, int newSize) {
    std::vector<cv::Point> resampled;
    int originalSize = contour.size();

    if (newSize >= originalSize) {
        // No need to resample if the new size is greater than or equal to the original
        return contour;
    }

    // Calculate the cumulative arc length along the contour
    std::vector<float> arcLength(originalSize);
    arcLength[0] = 0;
    for (int i = 1; i < originalSize; ++i) {
        arcLength[i] = arcLength[i - 1] + cv::norm(contour[i] - contour[i - 1]);
    }
    float totalLength = arcLength.back();

    // Generate new equally spaced arc lengths
    std::vector<float> newArcLengths(newSize);
    for (int i = 0; i < newSize; ++i) {
        newArcLengths[i] = (i * totalLength) / (newSize - 1);
    }

    // Resample the contour
    resampled.push_back(contour[0]); // First point remains the same
    int nextPointIndex = 1;
    for (int i = 1; i < newSize - 1; ++i) {
        while (newArcLengths[i] > arcLength[nextPointIndex] && nextPointIndex < originalSize - 1) {
            nextPointIndex++;
        }

        float t = (newArcLengths[i] - arcLength[nextPointIndex - 1]) / (arcLength[nextPointIndex] - arcLength[nextPointIndex - 1]);
        cv::Point newPoint = contour[nextPointIndex - 1] * (1 - t) + contour[nextPointIndex] * t;
        resampled.push_back(newPoint);
    }
    resampled.push_back(contour.back()); // Last point remains the same

    return resampled;
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> resampleContours(
    const std::vector<cv::Point>& contour1,
    const std::vector<cv::Point>& contour2) {
    
    if (contour1.size() == contour2.size()) {
        return {contour1, contour2};
    }

    const std::vector<cv::Point>* largerContour;
    const std::vector<cv::Point>* smallerContour;
    
    if (contour1.size() > contour2.size()) {
        largerContour = &contour1;
        smallerContour = &contour2;
    } else {
        largerContour = &contour2;
        smallerContour = &contour1;
    }

    // Resample the larger contour to match the size of the larger contour
    std::vector<cv::Point> resampledLargerContour = resampleContour(*largerContour, smallerContour->size());

    if (contour1.size() > contour2.size()) {
        return {resampledLargerContour, contour2};
    } else {
        return {contour1, resampledLargerContour};
    }
}

float calculateRotationFromContours(const std::vector<cv::Point>& srcContour, const std::vector<cv::Point>& dstContour) {
    // Calculate moments for both contours
    cv::Moments srcMoments = cv::moments(srcContour);
    cv::Moments dstMoments = cv::moments(dstContour);

    // Calculate the angle of orientation for each shape
    double srcAngle = 0.5 * atan2(2 * srcMoments.mu11, srcMoments.mu20 - srcMoments.mu02);
    double dstAngle = 0.5 * atan2(2 * dstMoments.mu11, dstMoments.mu20 - dstMoments.mu02);

    // Convert angles from radians to degrees
    srcAngle = srcAngle * 180.0 / CV_PI;
    dstAngle = dstAngle * 180.0 / CV_PI;

    // Calculate the rotation needed to align srcContour with dstContour
    float rotationNeeded = dstAngle - srcAngle;

    return rotationNeeded;
}

//--------------------------------------------------------------
void ofApp::setup() {
    
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofSetWindowTitle("olympicSans");



	if(!model.load("model")) {
		std::exit(EXIT_FAILURE);
	}

	std::vector<std::string> inputNames = {
		"serving_default_downsample_ratio:0",
		"serving_default_r1i:0",
		"serving_default_r2i:0",
		"serving_default_r3i:0",
		"serving_default_r4i:0",
		"serving_default_src:0"
	};
	std::vector<std::string> outputNames = {
		"StatefulPartitionedCall:0",
		"StatefulPartitionedCall:1",
		"StatefulPartitionedCall:2",
		"StatefulPartitionedCall:3",
		"StatefulPartitionedCall:4",
		"StatefulPartitionedCall:5"
	};
	model.setup(inputNames, outputNames);

	// parameters for the neural network
	float downsampleRatio = 0.25f;
	float videoWidth = 1920;
	float videoHeight = 1080;
	float batchSize = 1.0f;
	float numChannels = 3.0f;
	
	// model-specific inputs
	inputs = {
		cppflow::tensor({downsampleRatio}),
		cppflow::tensor({0.0f}),                         // r1i
		cppflow::tensor({0.0f}),                         // r2i
		cppflow::tensor({0.0f}),                         // r3i
		cppflow::tensor({0.0f}),                         // r4i
		cppflow::tensor({1,1920,1080,3}) // batch size width height channels
	};

	imgMask.allocate(videoWidth, videoHeight, OF_IMAGE_GRAYSCALE);
    grayImage.allocate(videoWidth, videoHeight);
    characterImage.allocate(videoWidth, videoHeight);
    fbo.allocate(videoWidth, videoHeight);
    pixels.allocate(1920,1080, OF_IMAGE_COLOR);
    syphonPixels.allocate(1920,1080, OF_IMAGE_COLOR_ALPHA);
    syphonImage.allocate(1920,1080, OF_IMAGE_COLOR);
	imgOut.allocate(videoWidth,videoHeight, OF_IMAGE_COLOR_ALPHA);

    
	imgOut.getTexture().setAlphaMask(imgMask.getTexture());
    
//    testFont.load("pixel.ttf", 90, true, true, true);
    
    testFont.load("Paris2024-Variable.ttf", 90, true, true, true);

    letter = '_';
    letterPlace = 0;
    
    for (int v = 0 ; v < numFramesToGather ; v++){
        saveImgs[v].allocate(1920, 1080, OF_IMAGE_COLOR);
        curVideoFrames[v].allocate(1920,1080, OF_IMAGE_COLOR);
    }
    
//    // Fill the array with capital letters
    for(int i = 0; i < numChars; ++i) {
        bestFits[i].letter ='A' + i;
        
        bool vflip = true;
        bool filled = true;
        testChar = testFont.getCharacterAsPoints(bestFits[i].letter, vflip, filled);

        filled = false;
        testCharContour = testFont.getCharacterAsPoints(bestFits[i].letter, vflip, filled);

        //std::vector<std::vector<cv::Point>> cvBlobs2;
        std::vector<cv::Point> cvBlob2;
        for(int k = 0; k <(int)testCharContour.getOutline().size(); k++){
            if( k!= 0)ofNextContour(true) ;
//            std::vector<cv::Point> cvBlob2;
            for(int ii = 0; ii < (int)testCharContour.getOutline()[k].size(); ii++){
                
                    cvBlob2.emplace_back(cv::Point(static_cast<int>(testCharContour.getOutline()[k].getVertices()[ii].x), static_cast<int>(testCharContour.getOutline()[k].getVertices()[ii].y)));
               // ofLog()<<"emplace2";
                }
               // cvBlobs2.push_back(cvBlob2);
           // ofLog()<<"push2";
            }
        
        
        // this is for
        
        // A 0
        // a 32
        // Q 16
        // q 48
        // d 35
        // g 38
        if (i == 16) {
            bestFits[i].charContours = cvBlobs2[2];
        }
       else if (i == 0 || i == 32 || i == 16 || i == 48 || i == 35 || i == 38) {
            bestFits[i].charContours = cvBlobs2[1];
        }
        else{
            bestFits[i].charContours = cvBlobs2[0];
        }

        else{
            
            bestFits[i].charContours = cvBlob2; // one big
        }
        
        for (int v = 0 ; v < numFramesToGather ; v++){
            bestFits[i].videoFrames[v].allocate(1920, 1080, OF_IMAGE_COLOR);
        }
    }
    

    ofSetFullscreen(false);
    //pix.allocate(video.getWidth(), video.getHeight());
    
    
        #ifdef USE_LIVE_VIDEO
    //        // setup video grabber
        mClient.setup();
        dir.setup();
        mClient.set(dir.getDescription(dir.size() - 1));
        #else
    ////        video.load("gymniastcs_trim.mp4");
        video.load("rock.mp4");
    ////
            video.play();
//            video.setLoopState(OF_LOOP_NONE);
        #endif

    //using Syphon app Simple Server, found at http://syphon.v002.info/
  //  mClient.set("","OBS");
}

//--------------------------------------------------------------
void ofApp::update() {

    
            #ifdef USE_LIVE_VIDEO
    
    if(mClient.isSetup( ) == false){
        return ;
    }
    
    
    fbo.begin();
    mClient.draw(0,0,ofGetWidth(), ofGetHeight() );
    fbo.end();
    
    fbo.readToPixels(pixels);
    
            #else
    
    video.update();
    if(video.isFrameNew()) {
        video.getPixels();
        
        fbo.begin();
        video.draw(0,0,ofGetWidth(), ofGetHeight() );
        fbo.end();
        
        fbo.readToPixels(pixels);
        
#endif
        
        // Print basic information
        //        std::cout << "Bits per Channel: " << pixels.getBitsPerChannel() << std::endl;
        //        std::cout << "Bits per Pixel: " << pixels.getBitsPerPixel() << std::endl;
        //        std::cout << "Bytes per Channel: " << (int)pixels.getBytesPerChannel() << std::endl;
        //        std::cout << "Bytes per Pixel: " << (int)pixels.getBytesPerPixel() << std::endl;
        //        std::cout << "Bytes Stride: " << pixels.getBytesStride() << std::endl;
        //        std::cout << "Number of Channels: " << pixels.getNumChannels() << std::endl;
        //
        //        // Print a specific pixel color (for example, the pixel at (0, 0))
        //        ofColor color = pixels.getColor(0, 0);
        //        std::cout << "Color at (0, 0): R=" << (int)color.r << ", G=" << (int)color.g << ", B=" << (int)color.b << std::endl;
        //
        //        // Print image dimensions and type
        //        std::cout << "Image Width: " << pixels.getWidth() << std::endl;
        //        std::cout << "Image Height: " << pixels.getHeight() << std::endl;
        //        std::cout << "Image Type: " << pixels.getImageType() << std::endl;
        //
        //        // Print data pointer
        //        std::cout << "Data Pointer: " << static_cast<void*>(pixels.getData()) << std::endl;
        //
        
        
        //      ofPixels rgbaPixels;// =
        
        
        
        if(gatherFrames<numFramesToGather){
            fbo.begin();
            
            
#ifdef USE_LIVE_VIDEO
            
            mClient.draw(0,0,ofGetWidth(), ofGetHeight() );
            
            
#else
            video.draw(0,0,ofGetWidth(), ofGetHeight());
            
            
#endif
            
            fbo.end();
            
            fbo.readToPixels(curVideoFrames[gatherFrames]);
            curVideoFrames[gatherFrames].setImageType(OF_IMAGE_COLOR);
            
            return;
        }
        
        
        // pixels = curVideoFrames[numFramesToGather-1];
        
        
        //    for (int y = 0; y < ofGetHeight(); ++y) {
        //        for (int x = 0; x < ofGetWidth(); ++x) {
        //            // Get the RGBA color
        //            ofColor rgbaColor = syphonPixels.getColor(x, y);
        //
        //            // Set the RGB color to the pixels object
        //            pixels.setColor(x, y, rgbaColor);
        //        }
        //    }
        
        pixels.setImageType(OF_IMAGE_COLOR);
        // pixels
        //        // Print basic informatio
        // prepare inputs
        auto input = ofxTF2::pixelsToTensor(pixels);
        auto inputCast = cppflow::cast(input, TF_UINT8, TF_FLOAT);
        inputCast = cppflow::mul(inputCast, cppflow::tensor({1/255.0f}));
        inputCast = cppflow::expand_dims(inputCast, 0);
        inputs[5] = inputCast;
        
        // run model
        auto outputs = model.runMultiModel(inputs);
        
        // process outputs
        inputs[1] = outputs[2];
        inputs[2] = outputs[3];
        inputs[3] = outputs[4];
        inputs[4] = outputs[5];
        auto foreground = outputs[1];
        foreground = cppflow::mul(foreground, cppflow::tensor({255.0f}));
        auto foregroundMod = cppflow::cast(foreground, TF_FLOAT, TF_UINT8);
        ofxTF2::tensorToImage(foreground, imgMask);
        imgMask.update();
        //imgOut.setFromPixels(video.getPixels());
        grayImage.setFromPixels(imgMask.getPixels());
        
        contourFinder.findContours(grayImage, 5, (camWidth*camHeight), 200, true);    // find holes
#ifndef USE_LIVE_VIDEO
    }
#endif


}

//--------------------------------------------------------------
void ofApp::draw() {

	float w = 1920;
	float h = 1080;

//	video.draw(0, 0, w, h);
	imgMask.draw(0, 0, w, h);
    
#ifdef USE_LIVE_VIDEO
    mClient.draw(0,0,w/6.,h/6.);

    syphonImage.setFromPixels(pixels);  // Load the pixels into the ofImage

    // Now you can draw syphonImage in your draw() method
    syphonImage.draw(w/6.,h/6.,w/6.,h/6.);  // Draw the image at position (0, 0)
# else
    video.draw(w/6.,h/6.,w/6.,h/6.);  // Draw the image at position (0, 0)
#endif
    
    fbo.begin();
    ofClear(0);
    //testChar.draw(w/2.,h/2. );

//    testCharContour.draw(350,250);/Users/charstiles/Movies/skate3.mp4
    fbo.end();
    fbo.draw(0,0);
    
    
   //get outline of olympian
    // open CV puts the biggest blob at zero position
    if (contourFinder.blobs.size() > 0){
        contourFinder.blobs[0].draw(0,0);
        if(contourFinder.blobs[0].boundingRect.getWidth()<50 && contourFinder.blobs[0].boundingRect.getHeight()<50 ){
            return;
        }
        // draw over the centroid if the blob is a hole
        ofSetColor(255);
        if(contourFinder.blobs[0].hole){
            ofDrawBitmapString(std::string(ofRandom(1,5), '?'),
                               contourFinder.blobs[0].boundingRect.getCenter().x,
                               contourFinder.blobs[0].boundingRect.getCenter().y);
            
        }
        centerXTemp = contourFinder.blobs[0].boundingRect.getCenter().x;
        centerYTemp = contourFinder.blobs[0].boundingRect.getCenter().y;
    }
    if(gatherFrames < numFramesToGather){
        gatherFrames++;
        return;
    }
    fbo.readToPixels(pix);
    characterImage.setFromPixels(pix);
    std::vector<std::vector<cv::Point>> points;
//    points = (std::vector<std::vector<cv::Point>>)contourFinder.blobs[0].pts;
    std::vector<std::vector<cv::Point>> cvBlobs;
//
    
    //make array of cv points for body
    if (contourFinder.blobs.size()>0) {
            std::vector<cv::Point> cvBlob;
        for (int j = 0 ; j< contourFinder.blobs[0].nPts; j++) {
                // Assuming we only need the x and y components of ofVec3f
                ofVec3f vec= contourFinder.blobs[0].pts[j];
                cvBlob.emplace_back(cv::Point(static_cast<int>(vec.x), static_cast<int>(vec.y)));
        
            }
            cvBlobs.push_back(cvBlob);
        // Calculate the centroid of the current blob
   
   
        }
    //bestFitsbodyContours = contourFinder.blobs[0];
    // make array of character points

    
    // matchShapes !!!!!
    for (int f=0 ; f < numChars ; f++ ){
        if(cvBlobs.size() >0 && bestFits[f].charContours.size() >0  ){
            
            if(cvBlobs[0].size() >0 ){
                float newSim = cv::matchShapes(cvBlobs[0],bestFits[f].charContours,cv::CONTOURS_MATCH_I2,0);
                if (newSim  < bestFits[f].sim){
                    bestFits[f].sim=newSim;
                   // saveImg.setFromPixels(video.getPixels());
                    /// cv::Mat homoMat =
                    
                    bestFits[f].centerX = centerXTemp;
                    bestFits[f].centerY = centerYTemp;
                    auto [resampledContour1, resampledContour2] = resampleContours(cvBlobs[0], bestFits[f].charContours);
                    rotation = calculateRotationFromContours(resampledContour1, resampledContour2);
                    homoMat = cvMatToOFMatrix( cv::estimateAffinePartial2D(resampledContour1, resampledContour2) );
                    ofFill();
                    ofSetColor(ofColor(255,0,0));
                    drawPointsOpenFrameworksStyle(resampledContour1);
                    ofSetColor(ofColor(0,255,0));
                    drawPointsOpenFrameworksStyle(resampledContour1);
                    ofNoFill();
                    bestFits[f].rotation = rotation;//atan2(homoMat(1, 0), homoMat(0, 0));;
                    bestFits[f].bodyContours =resampledContour1;
                    bestFits[f].charContours =resampledContour2;
                    for (int fram = 0 ; fram < numFramesToGather ; fram++){
                        bestFits[f].videoFrames[fram] = curVideoFrames[fram];
                    }
                    ofLog()<< ofToString(newSim) + " newSIM";
                    break;
                }
               
               ofDrawBitmapStringHighlight(ofToString(newSim) + " newLowsim", 12,39);
                
            }
        }
    }
    ofLog()<< " NEXT!";
    frameIndex=numFramesToGather;
    if(gatherFrames >= numFramesToGather){
        gatherFrames = 0;
        frameIndex = 0;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == 's'){
        mClient.set(dir.getDescription(dir.size() - 1));
        cout << "setting syphon";
    }
    float w = 1920;
    float h = 1080;
    
    // make the actual output image
    if (key == 'a'){
        ofLog() << "saveProgress";
        std::string sport = "folderName";
        //makeDir(sport);
            for (int c =0; c < numChars ; c++){
                
                for (int kk = 0; kk < numFramesToGather +1; kk++){
                    int k = kk;
                    if (kk >= numFramesToGather){
                        k=kk-1;
                    }
                    bestFits[c].videoFrames[k].setImageType(OF_IMAGE_COLOR);
                    saveImgs[k].setFromPixels(bestFits[c].videoFrames[k]);
                    ofClear(0);
                    
                    
                    ofPushMatrix();
                    ofTranslate((ofGetWidth()/2.)-bestFits[c].centerX, (ofGetHeight()/2.)-bestFits[c].centerY);
                    ofTranslate(bestFits[c].centerX,bestFits[c].centerY, 0);//move pivot to centre
                    //float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp = false);
                    
                    // mod rotation by 2PI
                    float rot = fmod(bestFits[c].rotation , 6.28318530718f);
                    
//                    if (rot > M_PI) {
//                        rot -= M_PI*2.;
//                    }
//
                    ofRotateZRad( ofMap(float(k),0.,numFramesToGather,0., rot) );//rotate from centre
                    
                    ofPushMatrix();
                    //ofTranslate(ofGetWidth()/2., ofGetHeight()/2.);
                    ofTranslate(-bestFits[c].centerX,-bestFits[c].centerY);
                    ofPushMatrix();
                    //ofTranslate((ofGetWidth()/2.)-centerX, (ofGetHeight()/2.)-centerY);
                    saveImgs[k].draw(0,0);//move back by the centre offset
                    
                    // draw the body contour over the last image for debugging !
                    if (kk >= numFramesToGather){
                            ofBeginShape();
                                    for(int i = 0; i < (int)bestFits[c].bodyContours.size(); i++){
                                        ofVertex(bestFits[c].bodyContours[i].x,
                                                 bestFits[c].bodyContours[i].y);
                                    }
                                
                            ofEndShape( true );
                    }
                    // img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
                    ofPopMatrix();
                    ofPopMatrix();
                    ofPopMatrix();
                    
                    
                    ofPushMatrix();
                        ofTranslate(25, 250, 0);
                        ofBeginShape();
                           
                                for(int i = 0; i < (int)bestFits[c].charContours.size(); i++){
                                    ofVertex(bestFits[c].charContours[i].x,
                                             bestFits[c].charContours[i].y);
                                }
                            
                        ofEndShape( true );
                    ofPopMatrix();
                    //testChar.draw(w/7., h/7. );
                    
                    //    testCharContour.draw(350,250);
                    //fbo.end();
                    // saveImg.setFromPixels(fbo.getPixels());
                    //        ofPushMatrix();
                    //
                    //        img.draw(0,0);
                    //        ofPopMatrix();
                    
                    img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
                    //bestFits[c].sim
                    if (bestFits[c].letter >= 'a'){
                        if(k == 0){
                            makeDir(sport+"/"+"_"+ ofToString(bestFits[c].letter) );
                        }
                        img.save(sport+"/"+"_"+ ofToString(bestFits[c].letter)+"/"+ofToString(bestFits[c].sim)+"_"+ofToString(kk,4,'0')+".png");
                    }
                    else{
                        if(k == 0){
                            makeDir(sport+"/"+ofToString(bestFits[c].letter) );
                        }
                        img.save(sport+"/"+ofToString(bestFits[c].letter)+"/"+ofToString(bestFits[c].sim)+"_"+ofToString(kk,4,'0')+".png");
                    }
//                    letterPlace = (letterPlace + 1);
//                    letter = capitalLetters[letterPlace];
//                    bool vflip = true;
//                    bool filled = true;
//                    testChar = testFont.getCharacterAsPoints(letter, vflip, filled);
//
//                    filled = false;
//                    testCharContour = testFont.getCharacterAsPoints(letter, vflip, filled);
//
                   // bestFits[c].sim=10.;
                }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
