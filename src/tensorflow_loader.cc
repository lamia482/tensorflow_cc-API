#include "tensorflow_loader.h"
#include "tf_api.h"

#ifdef TENSORFLOW_LOADER_H_

TensorflowLoader::TensorflowLoader(void)
{
  m_pTensorflowApi = new TensorflowApi();
}

TensorflowLoader::~TensorflowLoader(void)
{
  delete m_pTensorflowApi;
}

bool TensorflowLoader::createStatus(void)
{
  return m_pTensorflowApi->createStatus();
}

bool TensorflowLoader::loadModel(const std::string &model_file)
{
  return m_pTensorflowApi->loadModel(model_file);
}

bool TensorflowLoader::loadLabel(const std::string &label_file)
{
  return m_pTensorflowApi->loadLabel(label_file);
}

bool TensorflowLoader::feedPath(const std::string &image_file)
{
  return m_pTensorflowApi->feedPath(image_file);
}

bool TensorflowLoader::feedRawData(const unsigned char *data)
{
  return m_pTensorflowApi->feedRawData(data);
}

bool TensorflowLoader::readOperationName(const std::string &model_file)
{
  return m_pTensorflowApi->readOperationName(model_file);
}

std::vector<TensorflowLoaderPrediction> TensorflowLoader::doPredict(void)
{
  auto src = m_pTensorflowApi->doPredict();
  std::vector<TensorflowLoaderPrediction> res;
  if(!src.size())
    return res;
  res.resize(src.size());
  memcpy(&res[0], &src[0], res.size()*sizeof(TensorflowLoaderPrediction));
  return res;
}

std::string TensorflowLoader::readOption(const char *option)
{
  return (std::string)(m_pTensorflowApi->getReadOption()->read(option));
}

#endif // ! TENSORFLOW_LOADER_H_