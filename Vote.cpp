/*
 * Voting.cpp
 *
 * Author: Kuheli
 */

// g++ Vote.cpp -L /usr/lib -lopencv_core -lopencv_flann -o Vote -std=c++11

#include <tr1/unordered_map>
#include <iostream>
#include <iterator>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iomanip>
#include <vector>
#include <string>
#include <queue>

using namespace std;

//Declaring structures and classes

struct Element {
	string cents;
	vector<string> imageIds;
	float dist;
};

struct imgFreq {
	string imageId;
	float freq;
	imgFreq(string imgId, float f) {
		imageId = imgId;
		freq = f;
	}
};

class CompareFreq {
public:
	bool operator()(imgFreq& imgFreq1, imgFreq& imgFreq2) {
		return imgFreq1.freq < imgFreq2.freq;
	}
};

//Deleting structures

void deleteDistanceList(vector<Element*> &DistList) {
	cout << DistList.size() << endl;
	for (unsigned int i = 0; DistList.size(); i++) {
		cout << i << endl;
		string(DistList[i]->cents).swap(DistList[i]->cents);
		vector<string>().swap(DistList[i]->imageIds);
	}
	vector<Element*>().swap(DistList);
	return;
}

//Reading input file

void readDistanceList(vector<Element> &DistList, string fn) {
	string data_line;
	ifstream CL_file(fn.c_str());
	if (CL_file.is_open()) {
		while (getline(CL_file, data_line)) {
			Element e;
			istringstream buf(data_line);
			istream_iterator<string> beg(buf), end;
			vector<string> data(beg, end);
			e.cents = data[0];
			e.dist = atof(data[1].c_str());
			e.imageIds.insert(e.imageIds.end(),data.begin()+2,data.end());
			DistList.push_back(e);
		}
		CL_file.close();
	}
	CL_file.close();
}

//Writting output file

void printImageFreq(
		priority_queue<imgFreq, vector<imgFreq>, CompareFreq> &imgFreq_queue,
		const char *ImageFreqPath) {
	ofstream ImageFreq;
	ImageFreq.open(ImageFreqPath);
	while (!imgFreq_queue.empty()) {
		imgFreq iFreq = imgFreq_queue.top();
		ImageFreq << iFreq.imageId << " " << iFreq.freq << endl;
		imgFreq_queue.pop();
	}
	ImageFreq.close();
	return;
}


void computeVoting(vector<Element> &DistList,
		priority_queue<imgFreq, vector<imgFreq>, CompareFreq> &imgFreq_queue) {
	tr1::unordered_map<string, float> imageIdsFreq;
	//Getting the frequencies
	for (unsigned int i = 0; i < DistList.size(); i++) {
		for (unsigned int j = 0; j < DistList[i].imageIds.size(); j++) {
			string imageId = DistList[i].imageIds[j];
			imageIdsFreq[imageId] += 1.0-DistList[i].dist;
		}
	}
	
	for (auto it = imageIdsFreq.begin(); it != imageIdsFreq.end(); ++it)
		imgFreq_queue.push(imgFreq(it->first, it->second));
}

//Main function

int main(int argc, char** argv) {
	//Defining variables
	const char *ListPath = NULL;
	const char *ImageFreqPath = NULL;
	int i;
	//USAGE message
	if (argc != 5) {
		cout << "USAGE: ./GettingIndexes -i <ListPath> -o <ImageFreq>" << endl;
		cout
				<< "i                    <file_path> In  - path of final lists of distances."
				<< endl;
		cout
				<< "o                    <file_path> Out - path of the image frequencies."
				<< endl;
		return -1;
	}
	//Parsing Options
	for (i = 1; i < argc; i = i + 2) {
		if (strcmp(argv[i], "-i") == 0)
			ListPath=argv[i + 1];
		else if (strcmp(argv[i], "-o") == 0)
			ImageFreqPath=argv[i + 1];
		else {
			cout << "Wrong parameter" << endl;
		}
	}
	//Algorithm
	priority_queue<imgFreq, vector<imgFreq>, CompareFreq> imgFreq_queue;
	vector<Element> DistList;
	readDistanceList(DistList, ListPath);
	computeVoting(DistList, imgFreq_queue);
	printImageFreq(imgFreq_queue, ImageFreqPath);
	return 0;
}
