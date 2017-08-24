#ifndef TF_API_BASE_H_
#define TF_API_BASE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <map>
#include <tensorflow/cc/ops/standard_ops.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>

#include "timer.h"
#include "read_options.h"

typedef struct
{
  unsigned int category;
  unsigned int lefttopx;
  unsigned int lefttopy;
  unsigned int width;
  unsigned int height;
  float confidence;
}TensorflowApiPrediction;

class TensorflowApiBase
{
public:
  TensorflowApiBase(void){}
  virtual ~TensorflowApiBase(void){}
  virtual bool createStatus(void) = 0;
  virtual bool loadModel(const std::string &) = 0;
  virtual bool loadLabel(const std::string &) = 0;
  virtual bool feedPath(const std::string &i) = 0;
  virtual bool feedRawData(const unsigned char *) = 0;
  virtual bool readOperationName(const std::string &) = 0;
  virtual std::vector<TensorflowApiPrediction> doPredict(void) = 0;

protected:
  std::string m_InputTensorName;
  std::string m_OutputTensorBox;
  std::string m_OutputTensorScore;
  std::string m_OutputTensorClass;
  std::string m_OutputTensorNum;
  tensorflow::Tensor m_Sample;
  std::vector<std::pair<std::string, tensorflow::Tensor> > m_Samples;
  std::string m_LabelFile;
  std::map<int, std::string> m_Category;
  std::string m_ModelFile;
  tensorflow::Session *m_pSession;
  tensorflow::GraphDef m_GraphDef;
  int input_height, input_width, wanted_channels;
  float input_mean, input_std;
  float m_ProbThresh;
  std::vector<tensorflow::Tensor> m_ImageTensor;
  std::vector<tensorflow::Tensor> m_Outputs;
  clock_t m_Clock;
  Markt *m_pMarkt;
  ReadOptions *m_pReadOptions;
  bool m_bCreateStatus;
};


#endif // ! TF_API_BASE_H_
