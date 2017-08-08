#ifndef _TF_API_BASE_H_
#define _TF_API_BASE_H_

#include <iostream>
#include <string>
#include <tensorflow/cc/ops/standard_ops.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>

typedef std::vector<float> Sample;

class TensorflowLoaderBase
{
public:
	TensorflowLoaderBase(void){}
	virtual ~TensorflowLoaderBase(void){}
	virtual bool loadModel(const std::string &) = 0;
	virtual bool loadLabel(const std::string &) = 0;
	virtual bool feedSample(const Sample &) = 0;
	virtual bool feedPath(const std::string &i) = 0;
	virtual int doPredict(void) = 0;
	
protected:
	std::string m_InputTensorName;
	std::string m_OutputTensorName;
	std::string m_OutputTensorScore;
	std::string m_OutputTensorClass;
	std::string m_OutputTensorBox;
	tensorflow::Tensor m_Sample;
	std::vector<std::pair<std::string, tensorflow::Tensor> > m_Samples;
	std::string m_ModelFile;
	std::string m_LabelFile;
	tensorflow::Session *m_pSession;
	tensorflow::GraphDef m_GraphDef;
	int input_height, input_width;
	float input_mean, input_std;
	std::vector<tensorflow::Tensor> m_OutTensor;
	std::vector<tensorflow::Tensor> m_Outputs;
};


#endif