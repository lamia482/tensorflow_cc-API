#include "tf_api.h"
#include <iostream>

int main(int argc, char **argv)
{
	TensorflowLoader *tfLoader = new TensorflowLoader();

	// if(!tfLoader->loadModel("/home/e0024/workspace/cxx/tensorflow/test/model/nn_model_frozen.pb"))
	if(!tfLoader->loadModel("target_rcnn.pb"))
	{
		LOG(ERROR) << "Error: Fatal errors in loading model\n";
		return -1;
	}

	if(!tfLoader->feedSample(Sample({1.f, 1.f, 1.f, 1.f, 1.f})))
	{
		LOG(ERROR) << "Error: Fatal errors in feeding a sample\n";
		return -2;
	}
	
	if(!tfLoader->feedPath("images/image3.jpg"))
	{
		LOG(ERROR) << "Error: Fatal errors in reading from image\n";
		return -3;
	}
	
	if(-1 == tfLoader->doPredict())
	{
		LOG(ERROR) << "Error: Fatal error in predicting\n";
		return -4;
	}

	delete tfLoader;
	
	return 0;
}