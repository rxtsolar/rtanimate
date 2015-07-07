#ifndef MODEL_H
#define MODEL_H

#include <opencv2/opencv.hpp>

namespace model {

class Model {
public:
	Model() : p_width(16), p_height(16) {}
	void load(const char* iname);
	void write(const char* mname);
private:
	cv::Mat image;
	int p_width;
	int p_height;
};

} // end model


#endif
