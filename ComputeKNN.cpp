/*
 * ComputeKNN.cpp
 *
 * Author: Kuheli
 */

/* Command to compile the code: g++ ComputeKNN.cpp -L /usr/lib -lopencv_core -lopencv_flann -o ComputeKNN*/

#include <opencv/cv.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

using namespace cv;
using namespace std;

//Declaring structures

void readCodeBook(Mat &CodeBook, string CodeBookPath, int &num_cw) {
	ifstream fp(CodeBookPath.c_str());
	int dims;
	float val;
	if (fp.is_open()) {
		fp >> num_cw >> dims;
		CodeBook = Mat(num_cw, dims, CV_32F);
		for (int row = 0; row < num_cw; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				CodeBook.at<float>(row, col) = val;
			}
		}
		fp.close();
	}
	fp.close();
}

void write(Mat &Indices, Mat &Dists, string OutPath, string classId, string imageId, int num_cw, int num_query, int k, float dmin, float dmax, vector <string> idxString) {
	OutPath = OutPath + "/" + classId;
	mkdir(OutPath.c_str(), 0777);

	OutPath = OutPath + "/" + "IM" + classId + "_" + imageId;
	mkdir(OutPath.c_str(), 0777);

	/*ostringstream str1;
	str1 << num_cw;

	OutPath = OutPath + "/" + str1.str() + "cw";
	mkdir(OutPath.c_str(), 0777);*/

	ostringstream str2;
	str2 << k;
	OutPath = OutPath + "/" + str2.str() + "nn";
	mkdir(OutPath.c_str(), 0777);

	ostringstream f_query;
	f_query << std::setfill('0') << std::setw(5) << num_query;
	OutPath = OutPath + "/Query" + f_query.str() + ".dat";

	ofstream outFile(OutPath.c_str());

	int row, col;

	if (outFile.is_open()) {
		for (row = 0; row < Indices.rows; row++) {
			for (col = 0; col < Indices.cols; col++) 
			{
				//outFile << Indices.at<int>(row, col) << " " << Dists.at<float>(row, col) << " " << idxString.at(Indices.at<int>(row, col)) << endl;
				if (Dists.at<float>(row, col) < dmax) 
				{
					outFile << Indices.at<int>(row, col) << " " << (Dists.at<float>(row, col) - dmin) / (dmax-dmin) << " " << idxString.at(Indices.at<int>(row, col)) <<endl;
					//we make "Dists.at<float>(row, col)/dmax" to obtain the normalized distance
				} 
				else
					break;


			}
			if (col != Indices.cols)
				break;
		}
		outFile.close();
	}
	return;
}

inline void getIds(string &imageId, string &classId){
	int part=imageId.find("_");
	classId=imageId.substr(2,part-2);
	imageId=imageId.substr(part+1,imageId.size());
}

void searchKNN(string QueryPath, string CodeBookPath, int k, float dmin, float dmax, string idxPath, string OutPath) 
{
	Mat CodeBook;
	int num_cw, num_query = 0;
	readCodeBook(CodeBook, CodeBookPath, num_cw);
	Mat Query, Indices, Dists;
	cv::flann::Index kdtree(CodeBook, cv::flann::KDTreeIndexParams(4));
	int num_desc, dims;
	string classId = "", imageId = "", imageId_ant = "";
	float val;
	vector <string> idxString;
	string line1;
	ifstream infile1(idxPath.c_str());	
	if (infile1.is_open())
	{
		while (infile1>>line1)
		{
			idxString.push_back(line1);			
		}
		infile1.close();
	}
	else cout<< "Unable to open Detector1 kp file "<<endl;


	ifstream fp(QueryPath.c_str());
	if (fp.is_open()) {
		fp >> num_desc >> dims;
		dims--;//disconsidering the imgId from the detector
		Query = Mat(1, dims, CV_32F);
		for (int row = 0; row < num_desc; row++) {
			for (int col = 0; col < dims; col++) {
				fp >> val;
				Query.at<float>(0, col) = val;
			}
			fp >> imageId;
			getIds(imageId, classId);
			if (imageId.compare(imageId_ant) != 0) {
				cout << "Class: " << classId << " - Image: " << imageId << endl;
				imageId_ant = imageId;
				num_query = 0;
			}
			num_query++;
			kdtree.knnSearch(Query, Indices, Dists, k,
					cv::flann::SearchParams(64));
			write(Indices, Dists, OutPath, classId, imageId, num_cw, num_query, k, dmin, dmax, idxString);
		}
		fp.close();
	}
	fp.close();
}

int main(int argc, char *argv[]) {
	//Defining variables
	string QueryPath;
	string CodeBookPath;
	
	string OutputFilePath;
	string idxFilePath;
	int k = 0;
	float dmax = 0.0, dmin = 0.0;
	//USAGE message
	if (argc != 15) {
		cout << "USAGE: " << argv[0]
				<< " -q <QueryFilePath> -cb <CodeBookPath> -k <NearesNeighbour> -dmin <MinDistance> -dmax <MaxDistance> -idx <IndexFilePath> -o <OutputFilePath>"
				<< endl;
		cout << "q           <file_path>  In  - query's path" << endl;
		cout << "cb          <file_path>  In  - codebook's path" << endl;
		cout << "k           <int>        In  - number of desired nearest neighbors" << endl;
		cout << "dmin        <float>      In  - minimum distance over all the descriptors and codewords" << endl;
		cout << "dmax_e      <float>      In  - maximum distance over all the descriptors and codewords (+ some little error)" << endl;
		cout << "idx         <file_path>  In  - Idex file path" << endl;
		cout << "o           <file_path>  Out - output's path " << endl;
		return -1;
	}
	//Parsing Options
	for (int i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-q") == 0)
			QueryPath = argv[i + 1];
		else if (strcmp(argv[i], "-cb") == 0)
			CodeBookPath = argv[i + 1];
		else if (strcmp(argv[i], "-k") == 0)
			k = atoi(argv[i + 1]);
		else if (strcmp(argv[i], "-dmin") == 0)
			dmin = atof(argv[i + 1]);
		else if (strcmp(argv[i], "-dmax_e") == 0)
			dmax = atof(argv[i + 1]);
		else if (strcmp(argv[i], "-idx") == 0)
			idxFilePath = argv[i + 1];
		else if (strcmp(argv[i], "-o") == 0)
			OutputFilePath = argv[i + 1];
		else {
			cout << "Wrong parameter" << endl;
			return -1;
		}
	}
	//Main process
	searchKNN(QueryPath, CodeBookPath, k, dmin, dmax, idxFilePath, OutputFilePath);
}
