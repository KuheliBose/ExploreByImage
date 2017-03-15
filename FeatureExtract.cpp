/*
 * FeatureExtract.cpp
 *
 * Author: Kuheli
 */

// g++ `pkg-config --cflags opencv` -o FeatureExtract FeatureExtract.cpp `pkg-config --libs opencv`


#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	if ( argc != 11 ) 
    	{	
		cout<<"usage: "<< argv[0] <<" -i <Input Image>"<< " -det <Detector name>"<< " -des <Descriptor name>"<< " -od <Detector file>"<< " -odes <Descriptor name>"<< endl;
		cout << "i         	In   - Input image (.pgm, .png...)." << endl;
		cout << "det         	In   - Detector Name. Available: sift, surf." << endl;
		cout << "des         	In   - Descriptor Name. Available: sift, surf." << endl;
		cout << "od          	Out  - Output Detector file path." << endl;
		cout << "odes          	Out  - Output Descriptor file path." << endl;
	}
	else
	{
		const char *imPath = NULL;
		const char *detec = NULL;
		const char *desc = NULL;
		const char *kpOut = NULL;
		const char *descOut = NULL;	
		for (int i = 1; i < argc; i = i + 2) 
		{
			if (strcmp(argv[i], "-i") == 0)
				imPath = argv[i + 1];
			else if (strcmp(argv[i], "-det") == 0)
				detec = argv[i + 1];
			else if (strcmp(argv[i], "-des") == 0)
				desc = argv[i + 1];
			else if (strcmp(argv[i], "-od") == 0)
				kpOut = argv[i + 1];
			else if (strcmp(argv[i], "-odes") == 0)
				descOut = argv[i + 1];
			else
				cout<<"Wrong Parameter"<<endl;
		}

		cv::Mat objectImg = cv::imread(imPath, cv::IMREAD_GRAYSCALE);
		std::vector<cv::KeyPoint> objectKeypoints;
		cv::Mat objectDescriptors;
                		
		if (strcmp(detec, "sift")==0)
		{
			cv::FeatureDetector * detector = new cv::SIFT();
			detector->detect(objectImg, objectKeypoints);
		}
		else if (strcmp(detec, "surf")==0)
		{
			cv::FeatureDetector * detector = new cv::SURF(400.0, 1);
			detector->detect(objectImg, objectKeypoints);
		}
		else
			cout<<"Detector is not avilable."<<endl;

	
		

		if (strcmp(desc, "sift")==0)
		{
			cv::DescriptorExtractor * extractor = new cv::SIFT();
			extractor->compute(objectImg, objectKeypoints, objectDescriptors);
		}
		else if (strcmp(desc, "surf")==0)
		{
			cv::DescriptorExtractor * extractor = new cv::SURF(500.0,4,2, false, false);
			extractor->compute(objectImg, objectKeypoints, objectDescriptors);
		}
		else
			cout<<"Descriptor is not avilable."<<endl;

		ofstream outfile2 (descOut);
		if (outfile2.is_open())
		{
			outfile2 << (int)objectKeypoints.size() <<" "<<objectDescriptors.cols<<endl;
			for (int i=0; i<objectDescriptors.rows; ++i)
			{
				for (int j=0; j<objectDescriptors.cols; ++j)
				{
					outfile2 << objectDescriptors.at<float>(i,j)<<" ";
				}
				outfile2 <<endl;
			}
			outfile2.close();  
		}
		
	}
}
