#pragma once

#include <opencv.hpp>


void rotateImage(const cv::Mat &matSrc, cv::Mat &matDst, double angle)
{
	matDst = matSrc.clone();

	if (0 == angle)  { return; }

	//��ת����Ϊͼ������
	cv::Point2f center;
	center.y = (float)(matSrc.rows / 2.0 + 0.5);
	center.x = (float)(matSrc.cols / 2.0 + 0.5);

	//�����ά��ת�ķ���任����  
	cv::Mat M = cv::getRotationMatrix2D(center, angle, 1.0);

	//�任ͼ�񣬲��ú�ɫ�������ֵ  
	cv::warpAffine(matSrc, matDst, M, matDst.size(), CV_INTER_LINEAR, cv::BORDER_REPLICATE);
}


int detectPalmFromRawData(unsigned char *ptrRGB, const int imgHeight, const int imgWidth,
	                      const std::string strModelFile)
{
	if (NULL == ptrRGB)  { return -1; }

	//1.Convert raw data to mat(RGB --> BGR):
	cv::Mat img = cv::Mat::zeros(imgHeight, imgWidth, CV_8UC3);
	unsigned char *ptrDst = NULL;
	for (int y = 0; y < imgHeight; ++y)
	{
		ptrDst = img.ptr<unsigned char>(y);
		for (int x = 0; x < imgWidth; ++x)
		{
			ptrDst[3 * x] = ptrRGB[y*imgWidth + 3 * x + 2];
			ptrDst[3 * x + 1] = ptrRGB[y*imgWidth + 3 * x + 1];
			ptrDst[3 * x + 2] = ptrRGB[y*imgWidth + 3 * x];
		}
	}
	ptrDst = NULL;

	//2. Create and load the cascade classifier:
	std::string strClfFileName = strModelFile;
	cv::CascadeClassifier cascade;
	if (!cascade.load(strClfFileName))
	{
		return -2;
	}

	//3. Detect object:
	int delta_ang = 20;
	int countHit = 0;
	std::vector<cv::Rect> rects;
	cv::Mat imgRot;
	for (int r = 0; r < 180; r += delta_ang)
	{
		rotateImage(img, imgRot, r);

		//rects��ʾ��⵽��Ŀ������,
		//CV_HAAR_SCALE_IMAGE��ʾ�������ŷ���������⣬��������ͼ��
		rects.clear();
		cascade.detectMultiScale(imgRot, rects, 1.1, 3, 0, cv::Size(20, 20), cv::Size(400, 400));

		for (std::vector<cv::Rect>::const_iterator pRect = rects.begin(); pRect != rects.end(); ++pRect)
		{
			++countHit;
			cv::rectangle(imgRot, cv::Point(pRect->x, pRect->y), cvPoint(pRect->x + pRect->width, pRect->y + pRect->height), cv::Scalar(0, 0, 255));
		}

		//Return if detected:
		if (rects.size() > 0)
		{
			for (int y = 0; y < imgHeight; ++y)
			{
				ptrDst = imgRot.ptr<unsigned char>(y);
				for (int x = 0; x < imgWidth; ++x)
				{
					ptrRGB[3 * x] = ptrDst[y*imgWidth + 3 * x + 2];
					ptrRGB[3 * x + 1] = ptrDst[y*imgWidth + 3 * x + 1];
					ptrRGB[3 * x + 2] = ptrDst[y*imgWidth + 3 * x];
				}
			}
			ptrDst = NULL;

			return 1;
		}
	}

	return countHit;
}

