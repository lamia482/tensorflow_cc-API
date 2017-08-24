#ifndef TF_API_H_
#define TF_API_H_

#include "tf_api_base.h"
#include <glog/logging.h>

class TensorflowApi: public TensorflowApiBase
{
public:
  TensorflowApi(void);
  ~TensorflowApi(void);
  bool createStatus(void);
  bool loadModel(const std::string &model_file);
  bool loadLabel(const std::string &label_file);
  bool feedPath(const std::string &image_file);
  bool feedRawData(const unsigned char *data);
  bool readOperationName(const std::string &model_file);
  std::vector<TensorflowApiPrediction> doPredict(void);
  ReadOptions *getReadOption(void);
  void setGlogLevel(const std::string &);
  static std::map<std::string, google::LogSeverity> m_GlogLevelMap;
};

#endif // ! TF_API_H_
