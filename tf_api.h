#ifndef _TF_API_H_
#define _TF_API_H_

#ifdef __c_plus_plus
extern "C"
{
#endif

#include "tf_api_base.h"

class TensorflowLoader: public TensorflowLoaderBase
{
public:
	TensorflowLoader();
	~TensorflowLoader();
	bool loadModel(const std::string &model_file);
	bool feedSample(const Sample &sample);
	int doPredict(void);
};

#ifdef __c_plus_plus
}
#endif

#endif