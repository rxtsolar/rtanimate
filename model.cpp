#include <highgui.h>
#include <fstream>
#include <iostream>

#include "model.h"

using namespace std;
using namespace cv;
using namespace model;

// plane info
float pna = 0;
float pnb = 1;
float pnc = 0;
float pnn = 0;
float pdr = 0.5;
float pdg = 0.5;
float pdb = 0.5;
float psr = 0.1;
float psg = 0.1;
float psb = 0.1;
float prr = 0.1;
float prg = 0.1;
float prb = 0.1;
float pe = 0.9;
float pmu = 0;
// camera info
float cxf = 0;
float cyf = 1;
float czf = 1;
float cwf = 2;
float cdf = 3;
int cwp = 800;
int chp = 600;
// light info
float lxf = 0.5;
float lyf = 1;
float lzf = 0.5;
float lr = 1;
float lg = 1;
float lb = 1;
// sphere info
float smooth = 100;
float factor = 0.3;

void Model::load(const char* iname)
{
	image = imread(iname);
	if (!image.data) {
		cerr << "no image loaded" << endl;
		return;
	}
}

void Model::write(const char* mname)
{
	ofstream file(mname);
	int w = image.cols / p_width;
	int h = image.rows / p_height;
	float x_offset = 0.5*p_width - 0.5*image.cols;
	float z_offset = 0.5*p_height - 0.5*image.rows;
	float y_offset = 0.5*p_width;

	//time
	file << "mf 1" << endl;
	file << "mt 1" << endl;

	//camera
	file << "/ camera" << endl;
	file << "mc " << cxf*image.cols << ' ' << cyf*image.cols;
	file << ' ' << czf*image.rows << ' ' << -cxf*image.cols;
	file << ' ' << -cyf*image.cols << ' ' << -czf*image.rows;
	file << " 0 0 0 0 0 0 " << cdf*image.rows << ' ';
	file <<	cwf*image.cols << ' '; file << cwf*image.cols*0.75 << ' ';
	file << cwp << ' ' << chp << endl;

	//light
	file << "/ light" << endl;
	file << "ml " << lxf*image.cols << ' ' << lyf*image.cols;
	file << ' ' << lzf*image.rows << ' ' << lb << ' ' << lg;
	file << ' ' << lr << " 0 0 0 0 0 0" << endl;

	//plane
	file << "/ plane" << endl;
	file << "m " << pdb << ' ' << pdg << ' ' << pdr << ' ';
	file << psb << ' ' << psg << ' ' << psr << ' ';
	file << smooth << prb << ' ' << prg << ' ' << prr << endl;

	file << "mp " << pna << ' ' << pnb << ' ' << pnc << ' ';
	file << pnn << ' ' << pe << ' ' << pmu << endl;

	//sphere
	file << "/ spheres" << endl;
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			Mat iROI = image(Rect(i*p_width,
						j*p_height,
						p_width, p_height));
			Scalar diff = mean(iROI);
			diff[0] = diff[0] / 255.0;
			diff[1] = diff[1] / 255.0;
			diff[2] = diff[2] / 255.0;
			float spec = (diff[0] + diff[1] + diff[2])/3;

			file << "m " << diff[2] << ' ' << diff[1];
			file << ' ' << diff[0] << ' ' << spec << ' ';
			file << spec << ' ' << spec << ' ' << smooth;
			file << ' ' << spec << ' ' << spec << ' ';
			file << spec << endl;

			file << "ms " << i*p_width + x_offset << ' ';
			file << y_offset << ' ';
			file << j*p_height + z_offset << ' ';
			file << factor*p_width << ' ';
			//TODO need to modify
			file << "0 0 0 0 0 0 1 0 10" << endl;
		}
	}
}

