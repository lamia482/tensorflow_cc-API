#include "tf_api.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
	TensorflowLoader *tfLoader = new TensorflowLoader();

	std::string image_file = "images/image3.jpg";
	
	if(!tfLoader->loadModel("target_rcnn.pb"))
	{
		LOG(ERROR) << "Error: Fatal errors in loading model\n";
		return -1;
	}

	if(!tfLoader->loadLabel("category.txt"))
	{
		LOG(ERROR) << "Error: Fatal errors in loading category\n";
		return -2;
	}
	
	if(!tfLoader->feedSample(Sample({1.f, 1.f, 1.f, 1.f, 1.f})))
	{
		LOG(ERROR) << "Error: Fatal errors in feeding a sample\n";
		return -3;
	}
	
	
	if(!tfLoader->feedPath(image_file))
	{
		LOG(ERROR) << "Error: Fatal errors in reading from image\n";
		return -4;
	}
	
	std::vector<TensorflowLoaderPrediction> tfPred = tfLoader->doPredict();
	if(!tfPred.size())
	{
		LOG(ERROR) << "Error: Fatal error in predicting\n";
		return -5;
	}
	
	cv::Mat image = cv::imread(image_file);
	if(image.empty())
	{
		LOG(ERROR) << "Error: Imread image failed\n";
		return -6;		
	}
	for(int i=0;i<tfPred.size();++i)
	{
		cv::rectangle(image, cv::Rect(tfPred[i].lefttopx, tfPred[i].lefttopy, tfPred[i].width, tfPred[i].height), 
		cv::Scalar(0, 0, 255), 3);
	}
	cv::imwrite("save.jpg", image);
	
	delete tfLoader;
	
	return 0;
}