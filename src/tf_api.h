#ifndef _TF_API_H_
#define _TF_API_H_

#include "tf_api_base.h"

class TensorflowLoader: public TensorflowLoaderBase
{
public:
	TensorflowLoader(void);
	~TensorflowLoader(void);
	bool loadModel(const std::string &model_file);
	bool loadLabel(const std::string &label_file);
	bool feedSample(const Sample &sample);
	bool feedPath(const std::string &image_file);
	bool feedRawData(unsigned char *data);
	bool readOperationName(const std::string &);
	std::vector<TensorflowLoaderPrediction> doPredict(void);
};

#endif
