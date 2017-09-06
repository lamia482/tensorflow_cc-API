#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>
#include "tensorflow_loader.h"
#include "laMiaSocket.h"

int main(int argc, char **argv)
{
START:
  TensorflowLoader *tfLoader = new TensorflowLoader();
  if(!tfLoader->createStatus())
  {
    LOG(ERROR) << "Fatal: failed to create TensorflowLoader object";
               // << "since error code: " << tfLoader->errorCode()
               // << "error message: " << tfLoader->errorMsg();
    return 999;
  }
  
  laMiaSocket *ls = NULL;
  if(std::atoi(tfLoader->readOption("message").c_str()))
  {
    ls = new laMiaSocket();
    ls->setPort(4999);
    {
      ls->setRole(laMiaSocketTypeClient);
      std::string ip_addr;
      std::cerr << "Client selected, choose server ip: ";
      std::cin >> ip_addr;
      ls->connectServer(ip_addr.c_str());
    }
  }

  std::string image_file = tfLoader->readOption("image_file");

  if(!tfLoader->loadModel(tfLoader->readOption("model_file")))
  {
    LOG(ERROR) << "Error: Fatal errors in loading model\n";
    return -1;
  }

  if(std::atoi(tfLoader->readOption("readoperations").c_str()))
  {
    if(!tfLoader->readOperationName(""))
    {
      LOG(ERROR) << "Error: Fatal errors in reading operationsl\n";
      return -1;
    }
  }

  if(!tfLoader->loadLabel(tfLoader->readOption("label_file")))
  {
    LOG(ERROR) << "Error: Fatal errors in loading category\n";
    return -2;
  }

  std::vector<TensorflowLoaderPrediction> tfPred;
  cv::Mat image;
  if(std::atoi(tfLoader->readOption("opencv").c_str()))
  {
     image = cv::imread(image_file);
    if(image.empty())
    {
      LOG(ERROR) << "Error: Imread image failed\n";
      return -5;
    }
  }
  /*
  FEED_PATH:
  if(!tfLoader->feedPath(image_file))
  {
    LOG(ERROR) << "Error: Fatal errors in fetching image\n";
    return -7;
  }
  
  tfPred = tfLoader->doPredict();
  if(!tfPred.size())
  {
    LOG(ERROR) << "Error: Fatal error in predicting\n";
    return -8;
  }
  goto FEED_PATH;
  */
  unsigned char *data = new unsigned char[640*424*3];
  memset(data, 0, 640*424*3*sizeof(unsigned char));
  FILE *fp = NULL;
  fp = fopen(tfLoader->readOption("image_data").c_str(), "rb");
  if(fp == NULL)
  {
    LOG(ERROR) << "File: " << tfLoader->readOption("image_data") << " open failed\n";
    return -6;
  }
  fread(data, 1, 640*424*3*sizeof(unsigned char), fp);
  fclose(fp);
REPEATE:
  if(!tfLoader->feedRawData(data))
  {
    LOG(ERROR) << "Error: Fatal errors in fetching data\n";
    return -7;
  }

  tfPred = tfLoader->doPredict();
  if(!tfPred.size())
  {
    LOG(ERROR) << "Error: Fatal error in predicting\n";
    return -8;
  }
  if(std::atoi(tfLoader->readOption("message").c_str()))
  {
    char *buffer = new char[256];
    memset(buffer, 0, 256);
    buffer[0] = tfPred.size();
    memcpy((void*)&buffer[1], (void*)&tfPred[0], tfPred.size()*sizeof(TensorflowLoaderPrediction));
    ls->sendMessage(buffer);
    delete buffer;
  }

  if(std::atoi(tfLoader->readOption("opencv").c_str()))
  {
    for(int i=0;i<tfPred.size();++i)
    {
      cv::rectangle(image, cv::Rect(tfPred[i].lefttopx, tfPred[i].lefttopy, tfPred[i].width, tfPred[i].height),
      cv::Scalar(0, 0, 255), 3);
    }
    cv::imwrite("save.jpg", image);
  }
  
  goto REPEATE;
  
  if(data)
    delete data;
  if(tfLoader)
    delete tfLoader;
  if(ls)
    delete ls;

  // goto START;
  
  return 0;
}
