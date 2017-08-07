#include "tf_api.h"

TensorflowLoader::TensorflowLoader()
{
	m_InputTensorName = "inputs";
	m_OutputTensorName = "output_node";
}

TensorflowLoader::~TensorflowLoader()
{
	m_pSession->Close();
}

bool TensorflowLoader::loadModel(const std::string &model_file)
{
	tensorflow::Status status = tensorflow::NewSession(tensorflow::SessionOptions(), &m_pSession);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Create session failed, reason: " << status.ToString() << "\n";
		return false;
	}
	
	if(model_file == "")
	{
		LOG(ERROR) << "Error: Loading model file<" << model_file << "> failed, reason: model_file is empty\n";
		return false;
	}
	status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), model_file, &m_GraphDef);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Loading model file<" << model_file << "> failed, reason: " << status.ToString() << "\n";
		return false;
	}
	
	status = m_pSession->Create(m_GraphDef);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Create model grash failed, reason: " << status.ToString() << "\n";
		return false;
	}
	
	return true;
}

bool TensorflowLoader::feedSample(const Sample &sample)
{
	int sampleSize = sample.size();
	if(sampleSize == 0)
	{
		LOG(WARNING) << "Warning: Sample being empty\n";
		return true;
	}
	else if(sampleSize < 0)
	{
		LOG(ERROR) << "Error: Sample being ruined\n";
		return false;
	}
	
	m_Sample = tensorflow::Tensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, sampleSize}));
	
	for(int i=0;i<sampleSize;++i)
		m_Sample.tensor<float, 2>()(0, i) = sample[i];
	
	m_Samples.push_back(std::pair<std::string, tensorflow::Tensor>(m_InputTensorName, m_Sample));
	
	return true;
}

int TensorflowLoader::doPredict(void)
{
	tensorflow::Status status = m_pSession->Run(m_Samples, {m_OutputTensorName}, {}, &m_Outputs);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Running session failed\n";
		return -1;
	}
	tensorflow::Tensor resTensor = m_Outputs[0];
	int outputSize = resTensor.dim_size(1);
	
	int resPrediction = 0;
	float resPredictionProb = 0.0f;
	for(int i=0;i<outputSize;++i)
	{
		if(resPredictionProb <= resTensor.tensor<float, 2>()(0, i))
		{
			resPredictionProb = resTensor.tensor<float, 2>()(0, i);
			resPrediction = i;
		}
	}
	
	LOG(INFO) << "\nclass ID: " << resPrediction << "\tProb: " << resPredictionProb << "\n";
	
	return resPrediction;
}



