#ifndef _TENSORFLOW_LOADER_H
#define _TENSORFLOW_LOADER_H

#include <string>
#include <vector>

typedef struct
{
  unsigned int category;
	unsigned int lefttopx;
	unsigned int lefttopy;
	unsigned int width;
	unsigned int height;
	float confidence;
}TensorflowLoaderPrediction;

class TensorflowApi;

class TensorflowLoader
{
public:
  TensorflowLoader(void);
  ~TensorflowLoader(void);
  bool loadModel(const std::string &);
  bool loadLabel(const std::string &);
  bool feedPath(const std::string &);
  bool feedRawData(const unsigned char *);
  bool readOperationName(const std::string &);
  std::vector<TensorflowLoaderPrediction> doPredict(void);
private:
  TensorflowApi *m_pTensorflowApi;
};


#endif
