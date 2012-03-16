#ifndef MODEL_H
#define MODEL_H

#include <cv.h>

namespace model {

class Model {
public:
	Model() : p_width(8), p_height(8) {}
	void load(const char* iname);
	void write(const char* mname);
private:
	cv::Mat image;
	int p_width;
	int p_height;
};

} // end model


#endif
