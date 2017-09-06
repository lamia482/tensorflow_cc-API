#include "tf_api.h"

#ifdef TF_API_H_

TensorflowApi::TensorflowApi()
{
  m_bCreateStatus = true; 
  m_InputTensorName = "image_tensor:0";
  m_OutputTensorBox = "detection_boxes:0";
  m_OutputTensorScore = "detection_scores:0";
  m_OutputTensorClass = "detection_classes:0";
  m_OutputTensorNum = "num_detections:0";
  
  // m_GlogLevelMap["DEBUG"] = google::DEBUG;
  m_GlogLevelMap["INFO"] = google::INFO;
  m_GlogLevelMap["WARNING"] = google::WARNING;
  m_GlogLevelMap["ERROR"] = google::ERROR;
  m_GlogLevelMap["FATAL"] = google::FATAL;
  
  m_pReadOptions = new ReadOptions("resource/default.cfg");
  if(!m_pReadOptions->readPermitted())
  {
    m_bCreateStatus = false;
    return;
  }
  m_pMarkt = new Markt(std::atol(m_pReadOptions->read("cpu_frequency")));
  google::InitGoogleLogging(m_pReadOptions->read("exe_name"));
  setGlogLevel((std::string)(m_pReadOptions->read("log_level")));  
  
  input_width = std::atoi(m_pReadOptions->read("raw_data_width"));
  input_height = std::atoi(m_pReadOptions->read("raw_data_height"));
  wanted_channels = std::atoi(m_pReadOptions->read("raw_data_channels"));
  m_ProbThresh = std::atof(m_pReadOptions->read("thresh")); 
  if(wanted_channels != 3)
  {
    m_bCreateStatus = false;
    return;
  }
  
  LOG(INFO) << "Info: initial done...";
}

TensorflowApi::~TensorflowApi()
{
  m_pSession->Close();
  if(m_pMarkt)
    delete m_pMarkt;
  if(m_pReadOptions)
    delete m_pReadOptions;
  google::ShutdownGoogleLogging();
}

ReadOptions *TensorflowApi::getReadOption(void)
{
  return m_pReadOptions;
}

std::map<std::string, google::LogSeverity> TensorflowApi::m_GlogLevelMap;

void TensorflowApi::setGlogLevel(const std::string &logLevel)
{
  google::SetStderrLogging(m_GlogLevelMap[logLevel]);
}

bool TensorflowApi::createStatus(void)
{
  return m_bCreateStatus;
}

bool TensorflowApi::loadModel(const std::string &model_file)
{
  // tensorflow::GPUOptions gpuConfig;
  // gpuConfig.set_per_process_gpu_memory_fraction(.5);
  tensorflow::SessionOptions sessionConfig;
  sessionConfig.config.mutable_gpu_options()->set_per_process_gpu_memory_fraction(std::atof(m_pReadOptions->read("gpu_fraction")));
  sessionConfig.config.mutable_gpu_options()->set_allow_growth(true);
  tensorflow::Status status = tensorflow::NewSession(sessionConfig, &m_pSession);
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
  if(tensorflow::StringPiece(model_file).ends_with(".pb"))
  {
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
  }
  else if(tensorflow::StringPiece(model_file).ends_with(".ckpt"))
  {
    LOG(WARNING) << "Warning: not support file with suffix of ckpt for now <" << model_file << ">";
    return false;
  }
  else
  {
    LOG(ERROR) << "Error: file with unrecognized suffix <" << model_file << ">";
    return false;
  }
  LOG(INFO) << "Info: load model from <" << model_file << ">";
  return true;
}

bool TensorflowApi::loadLabel(const std::string &label_file)
{
  m_LabelFile = label_file;
  std::ifstream labels(m_LabelFile);
  if (!labels) {
    LOG(WARNING) << "Warning: No file: " << label_file << " exists\n";
    return false;
  }
  if(tensorflow::StringPiece(label_file).ends_with(".txt"))
  {
    std::string line;
    int idx = 1;
    while(std::getline(labels, line))
    {
      m_Category[idx++] = line;
    }
    labels.close();
  }
  else if(tensorflow::StringPiece(label_file).ends_with(".pbtxt"))
  {
    LOG(WARNING) << "Warning: not support label.pbtxt for now";
    return true;
  }

  return true;
}

bool TensorflowApi::feedPath(const std::string &image_file)
{
  m_Clock = clock();
  m_pMarkt->mark1();
  // tensorflow::ops::DecodeRaw()
  auto root = tensorflow::Scope::NewRootScope();
  tensorflow::string input_name = "file_reader";
  tensorflow::string original_name = "identity";
  tensorflow::string output_name = "normalized";
  auto file_reader = tensorflow::ops::ReadFile(root.WithOpName(input_name), image_file);
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
  auto dims_expander = tensorflow::ops::ExpandDims(root.WithOpName("dims_expander"), original_image, 0);
  // auto resized = tensorflow::ops::ResizeBilinear(root, dims_expander, \
          tensorflow::ops::Const(root.WithOpName("size"), {input_height, input_width}));
  // auto div = tensorflow::ops::Div(root.WithOpName(output_name), tensorflow::ops::Sub(root, resized, {input_mean}), {input_std});
  // auto uint8_caster = tensorflow::ops::Cast(root.WithOpName("uint8_caster"), resized, tensorflow::DT_UINT8);

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
  status = session->Run({}, {"dims_expander"}, {}, &m_ImageTensor);
  if(!status.ok())
  {
    LOG(ERROR) << "Error: Running image session failed, reason: " << status.ToString() << "\n";
    return false;
  }
  session->Close();
  return true;
}

bool TensorflowApi::feedRawData(const unsigned char *data)
{
  m_Clock = clock();
  m_pMarkt->mark1();
  auto imageTensor = tensorflow::Tensor(tensorflow::DT_UINT8, {{1, input_height, input_width, wanted_channels}});
  std::copy_n(data, input_height*input_width*wanted_channels, imageTensor.flat<unsigned char>().data());

  m_ImageTensor.resize(1);
  m_ImageTensor[0] = imageTensor;

  return true;
}

bool TensorflowApi::readOperationName(const std::string &model_file)
{
  for (int i = 0; i < m_GraphDef.node_size(); ++i)
  {
    auto node = m_GraphDef.node(i);
    LOG(INFO) << node.name() << "  " << node.op() << "\n";
  }

  return true;
}

std::vector<TensorflowApiPrediction> TensorflowApi::doPredict(void)
{
  std::vector<TensorflowApiPrediction> res;
  
  tensorflow::Status status = m_pSession->Run({{m_InputTensorName, m_ImageTensor[0]}}, {m_OutputTensorBox, m_OutputTensorScore, m_OutputTensorClass, m_OutputTensorNum}, {}, &m_Outputs);
  if(!status.ok())
  {
    LOG(ERROR) << "Error: Running session failed, reason: " << status.ToString() << "\n";
    return res;
  }
  m_pMarkt->mark2();
  auto boxTensor = m_Outputs[0].flat<float>();
  auto scoreTensor = m_Outputs[1].flat<float>();
  auto classTensor = m_Outputs[2].flat<float>();
  auto numTensor = m_Outputs[3].flat<float>();

  int resPrediction = 1;
  float resPredictionProb = 0.0f;
  int numPrediction = 0;
  for(int i=0;i<numTensor(0);++i)
  {
    if(scoreTensor(i) < m_ProbThresh)
      break;
    numPrediction = i+1;
    // [y1, x1, y2, x2];
    LOG(INFO) << " Index: " << i << "\tclass ID: " << classTensor(i) \
          << "\tCategory: " << m_Category[classTensor(i)] \
          << " Prob: " << scoreTensor(i) \
          << " [" << boxTensor(i*4+1)*input_width << ", " << boxTensor(i*4+0)*input_height \
          << ", " << boxTensor(i*4+3)*input_width - boxTensor(i*4+1)*input_width \
          << ", " << boxTensor(i*4+2)*input_height - boxTensor(i*4+0)*input_height << "]";
    if(resPredictionProb <= scoreTensor(i))
    {
      resPredictionProb = scoreTensor(i);
      resPrediction = classTensor(i);
    }
  }

  LOG(INFO) << " result -->  class ID: " << resPrediction \
        << "\tcategory: " << m_Category[resPrediction] \
        << " prob: " << resPredictionProb \
        // << " time: " << 1.f*(clock() - m_Clock)/1000000.f << " seconds" 
        << " time: " << m_pMarkt->sectime() << " seconds" \
        << "\n\n"; 

  if(numPrediction > 0)
  {
    res.resize(numPrediction);
    for(int i=0;i<numPrediction;++i)
    {
      res[i].category   = classTensor(i);
      res[i].lefttopy   = boxTensor(i*4+0) * input_height;
      res[i].lefttopx   = boxTensor(i*4+1) * input_width;
      res[i].height     = boxTensor(i*4+2) * input_height - res[i].lefttopy;
      res[i].width      = boxTensor(i*4+3) * input_width - res[i].lefttopx;
      res[i].confidence = scoreTensor(i);
    }
  }

  return res;
}

#endif // ! TF_API_H_
