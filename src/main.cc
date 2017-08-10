#include "tf_api.h"
#include <opencv2/opencv.hpp>

int main(int argc, char **argv)
{
START:
	TensorflowLoader *tfLoader = new TensorflowLoader();

	std::string image_file = "images/image3.jpg";

	if(!tfLoader->loadModel("resource/target_rcnn.pb"))
	{
		LOG(ERROR) << "Error: Fatal errors in loading model\n";
		return -1;
	}

	/*
	if(!tfLoader->readOperationName(""))
	{
		LOG(ERROR) << "Error: Fatal errors in reading operationsl\n";
		return -1;
	}
	*/

	if(!tfLoader->loadLabel("resource/category.txt"))
	{
		LOG(ERROR) << "Error: Fatal errors in loading category\n";
		return -2;
	}

	if(!tfLoader->feedSample(Sample({1.f, 1.f, 1.f, 1.f, 1.f})))
	{
		LOG(ERROR) << "Error: Fatal errors in feeding a sample\n";
		return -3;
	}

  std::vector<TensorflowLoaderPrediction> tfPred;
	if(!tfLoader->feedPath(image_file))
	{
		LOG(ERROR) << "Error: Fatal errors in reading from image\n";
		return -4;
	}
	 tfPred = tfLoader->doPredict();
	if(!tfPred.size())
	{
		LOG(ERROR) << "Error: Fatal error in predicting\n";
		return -8;
	}

	cv::Mat image = cv::imread(image_file);
	if(image.empty())
	{
		LOG(ERROR) << "Error: Imread image failed\n";
		return -5;
	}
	/*
	unsigned char *data = new unsigned char[640*424*3];
	memset(data, 0, 640*424*3*sizeof(unsigned char));
	FILE *fp = NULL;
	fp = fopen("images/image3.data", "rb");
	if(fp == NULL)
	{
		LOG(ERROR) << "File: images/image3.data open failed\n";
		return -6;
	}
	fread(data, 1, 640*424*3*sizeof(unsigned char), fp);
	*/
REPEATE:
	if(!tfLoader->feedRawData(image.data))
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
	// goto REPEATE;

	for(int i=0;i<tfPred.size();++i)
	{
		cv::rectangle(image, cv::Rect(tfPred[i].lefttopx, tfPred[i].lefttopy, tfPred[i].width, tfPred[i].height),
		cv::Scalar(0, 0, 255), 3);
	}
	cv::imwrite("save.jpg", image);


	// delete data;
	delete tfLoader;

	return 0;
}
