#ifndef _TF_API_BASE_H_
#define _TF_API_BASE_H_

#include <iostream>
#include <string>
#include <glog/logging.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>

typedef std::vector<float> Sample;

class TensorflowLoaderBase
{
public:
	TensorflowLoaderBase(){}
	virtual ~TensorflowLoaderBase(){}
	virtual bool loadModel(const std::string &model_file) = 0;
	virtual bool feedSample(const Sample &sample) = 0;
	virtual int doPredict(void) = 0;
	
protected:
	std::string m_InputTensorName;
	std::string m_OutputTensorName;
	tensorflow::Tensor m_Sample;
	std::vector<std::pair<std::string, tensorflow::Tensor> > m_Samples;
	tensorflow::Session *m_pSession;
	tensorflow::GraphDef m_GraphDef;
	std::vector<tensorflow::Tensor> m_Outputs;
};


#endif