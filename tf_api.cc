#include "tf_api.h"

TensorflowLoader::TensorflowLoader()
{
	m_InputTensorName = "image_tensor:0";
	m_OutputTensorName = "";
	m_OutputTensorScore = "detection_scores:0";
	m_OutputTensorClass = "detection_classes:0";
	m_OutputTensorBox = "detection_boxes:0";
	input_width = 448;
	input_height = 448;
	input_mean = 128;
	input_std = 128;
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
	m_ModelFile = model_file;
	status = m_pSession->Create(m_GraphDef);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Create model grash failed, reason: " << status.ToString() << "\n";
		return false;
	}
	
	return true;
}

bool TensorflowLoader::loadLabel(const std::string &label_file)
{
	
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

bool TensorflowLoader::feedPath(const std::string &image_file)
{
	// tensorflow::ops::DecodeRaw()
	auto root = tensorflow::Scope::NewRootScope();
	tensorflow::string input_name = "file_reader";
	tensorflow::string original_name = "identity";
	tensorflow::string output_name = "normalized";
	auto file_reader = tensorflow::ops::ReadFile(root.WithOpName(input_name), image_file);
	const int wanted_channels = 3;
	tensorflow::Output image_reader;
	if (tensorflow::StringPiece(image_file).ends_with(".png")) 
	{
		image_reader = tensorflow::ops::DecodePng(root.WithOpName("png_reader"), file_reader, tensorflow::ops::DecodePng::Channels(wanted_channels));
	} 
	else if (tensorflow::StringPiece(image_file).ends_with(".gif")) 
	{
		image_reader = tensorflow::ops::DecodeGif(root.WithOpName("gif_reader"), file_reader);
	} 
	else if (tensorflow::StringPiece(image_file).ends_with(".jpg"))
	{
		// Assume if it's neither a PNG nor a GIF then it must be a JPEG.
		image_reader = tensorflow::ops::DecodeJpeg(root.WithOpName("jpg_reader"), file_reader, tensorflow::ops::DecodeJpeg::Channels(wanted_channels));
	}
	
	auto original_image = tensorflow::ops::Identity(root.WithOpName(original_name), image_reader);
	// auto float_caster = tensorflow::ops::Cast(root.WithOpName("float_caster"), original_image, tensorflow::DT_FLOAT);
	auto dims_expander = tensorflow::ops::ExpandDims(root, original_image, 0);
	auto resized = tensorflow::ops::ResizeBilinear(root, dims_expander, 
					tensorflow::ops::Const(root.WithOpName("size"), {input_height, input_width}));
	// auto div = tensorflow::ops::Div(root.WithOpName(output_name), tensorflow::ops::Sub(root, resized, {input_mean}), {input_std});
	auto uint8_caster = tensorflow::ops::Cast(root.WithOpName("uint8_caster"), resized, tensorflow::DT_UINT8);

	tensorflow::Status status;
	tensorflow::GraphDef graph;
	status = root.ToGraphDef(&graph);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Loading image tensor to graph failed, reason: " << status.ToString() << "\n";
		return false;
	}
	std::unique_ptr<tensorflow::Session> session(tensorflow::NewSession(tensorflow::SessionOptions()));
	status = session->Create(graph);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Creating image graph failed, reason: " << status.ToString() << "\n";
		return false;
	}
	status = session->Run({}, {"uint8_caster", original_name}, {}, &m_OutTensor);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Running image session failed, reason: " << status.ToString() << "\n";
		return false;
	}
	return true;
}

int TensorflowLoader::doPredict(void)
{
	tensorflow::Status status = m_pSession->Run({{m_InputTensorName, m_OutTensor[0]}}, {m_OutputTensorClass, m_OutputTensorScore, m_OutputTensorBox}, {}, &m_Outputs);
	if(!status.ok())
	{
		LOG(ERROR) << "Error: Running session failed, reason: " << status.ToString() << "\n";
		return -1;
	}
	tensorflow::Tensor classTensor = m_Outputs[0];
	tensorflow::Tensor scoreTensor = m_Outputs[1];
	tensorflow::Tensor boxTensor = m_Outputs[2];
	int classOutputSize = classTensor.dim_size(1);
	int scoreOutputSize = scoreTensor.dim_size(1);
	int boxOutputSize = boxTensor.dim_size(1);
	
	int resPrediction = 0;
	float resPredictionProb = 0.0f;
	for(int i=0;i<classOutputSize;++i)
	{
		LOG(INFO) << "\n index: " << i << "\tclass ID: " << classTensor.tensor<float, 2>()(0, i) << "\tProb: " << scoreTensor.tensor<float, 2>()(0, i) << "\n";
		if(resPredictionProb <= scoreTensor.tensor<float, 2>()(0, i))
		{
			resPredictionProb = scoreTensor.tensor<float, 2>()(0, i);
			resPrediction = classTensor.tensor<float, 2>()(0, i);
		}
	}
	
	LOG(INFO) << "\nresult  -->  class ID: " << resPrediction << "\tProb: " << resPredictionProb << "\n";
	
	return resPrediction;
}



