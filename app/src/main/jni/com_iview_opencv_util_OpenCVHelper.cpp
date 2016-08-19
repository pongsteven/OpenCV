//
// Created by pengqin on 2016/8/15.
//

#include "com_iview_opencv_util_OpenCVHelper.h"
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <android/bitmap.h>
#include <android/log.h>
#include <opencv2/opencv.hpp>
#define  LOG_TAG    "OpenCv"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define HEIGHT_CAMERA  640
#define WIDTH_CAMERA  480

#define RGB565_R(p) ((((p) & 0xF800) >> 11) << 3)
#define RGB565_G(p) ((((p) & 0x7E0 ) >> 5)  << 2)
#define RGB565_B(p) ( ((p) & 0x1F  )        << 3)
#define MAKE_RGB565(r,g,b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

#define RGBA_A(p) (((p) & 0xFF000000) >> 24)
#define RGBA_R(p) (((p) & 0x00FF0000) >> 16)
#define RGBA_G(p) (((p) & 0x0000FF00) >>  8)
#define RGBA_B(p)  ((p) & 0x000000FF)
#define HEIGHT_CAMERA  640
#define WIDTH_CAMERA  480
#define MAKE_RGBA(r,g,b,a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

using namespace cv;

/**
 * 返回值 char* 这个代表char数组的首地址
 *  Jstring2CStr 把java中的jstring的类型转化成一个c语言中的char 字符串
 */
char* Jstring2CStr(JNIEnv* env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String"); //String
    jstring strencode = env->NewStringUTF("GB2312"); // 得到一个java字符串 "GB2312"
    jmethodID mid = env->GetMethodID(clsstring, "getBytes",
            "(Ljava/lang/String;)[B"); //[ String.getBytes("gb2312");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod( jstr, mid,
            strencode); // String .getByte("GB2312");
    jsize alen = env->GetArrayLength( barr); // byte数组的长度
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char*) malloc(alen + 1); //"\0"
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements( barr, ba, 0); //
    return rtn;
}

void rotateImage(const cv::Mat &matSrc, cv::Mat &matDst, double angle)
{
__android_log_print(ANDROID_LOG_DEBUG, "abc",  "angle: %f", angle);
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
__android_log_print(ANDROID_LOG_DEBUG, "abc",  "out, angle: %f", angle);
}

int detectPalmFromRawData(unsigned char *ptrRGB, const int imgHeight, const int imgWidth, const std::string strModelFile)
{
    __android_log_print(ANDROID_LOG_DEBUG, "abc",  "Run: %s", "Enter");

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
	std::string strClfFileName = "/sdcard/cascade_14_392_140_HogRGB_0.5_2.xml";
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
		__android_log_print(ANDROID_LOG_DEBUG, "abc",  "rects.size(): %d", rects.size());

		for (std::vector<cv::Rect>::const_iterator pRect = rects.begin(); pRect != rects.end(); ++pRect)
		{
			++countHit;
			cv::rectangle(imgRot, cv::Point(pRect->x, pRect->y), cvPoint(pRect->x + pRect->width, pRect->y + pRect->height), cv::Scalar(0, 0, 255), 20);
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

void extractBitmap(JNIEnv *env,jobject bitmap,unsigned char* ptr){



}
void extract(JNIEnv *env, jobject zBitmap, unsigned char * ptr) {
	if (zBitmap == NULL) {
		return;
	}
	// Get bitmap info
	AndroidBitmapInfo info;
	memset(&info, 0, sizeof(info));
	AndroidBitmap_getInfo(env, zBitmap, &info);
	// Lock the bitmap to get the buffer
	void * pixels = NULL;
	int res = AndroidBitmap_lockPixels(env, zBitmap, &pixels);
	int x = 0, y = 0;
	// From top to bottom
	int width = info.width;
	int height = info.height;

	for (y = 0; y < height; ++y) {
		// From left to right
		for (x = 0; x < width; ++x) {
			int a = 0, r = 0, g = 0, b = 0;
			void *pixel = NULL;
			// Get each pixel by format
			if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
				pixel = ((uint16_t *) pixels) + y * width + x;
				uint16_t v = *(uint16_t *) pixel;
				int idx = (y * width + x) * 3;
				r = RGB565_R(v);
				g = RGB565_G(v);
				b = RGB565_B(v);
				ptr[idx] = r;
				ptr[idx + 1] = g;
				ptr[idx + 2] = b;
			} else { // RGBA
				pixel = ((uint32_t *) pixels) + y * width + x;
				int idx = (y * width + x) * 3;
				uint32_t v = *(uint32_t *) pixel;
				a = RGBA_A(v);
				r = RGBA_R(v);
				g = RGBA_G(v);
				b = RGBA_B(v);
				ptr[idx] = r;
                ptr[idx + 1] = g;
                ptr[idx + 2] = b;
			}
		}
	}
	AndroidBitmap_unlockPixels(env, zBitmap);

}

void getResultBitmap(JNIEnv *env, jobject zBitmap, unsigned char * ptr){
	if (zBitmap == NULL) {
		return ;
	}
	// Get bitmap info
	AndroidBitmapInfo info;
	memset(&info, 0, sizeof(info));
	AndroidBitmap_getInfo(env, zBitmap, &info);
	// Lock the bitmap to get the buffer
	void * pixels = NULL;
	int res = AndroidBitmap_lockPixels(env, zBitmap, &pixels);
	int x = 0, y = 0;
	// From top to bottom
	int width = info.width;
	int height = info.height;

		for (y = 0; y < height; ++y) {
                 		// From left to right
                 		for (x = 0; x < width; ++x) {

                 			uint32_t *pixel = NULL;
                 			int idx = (y * width + x) * 3;
                 			// Get each pixel by format
                 			int tr = ptr[idx];
                             int tg = ptr[idx + 1];
                             int tb = ptr[idx + 2];
                             int color = 255 << 24 | tr << 16 | tg << 8 | tb;
                             //(a) << 24) | ((r) << 16) | ((g) << 8) | (b)
                 		    pixel = ((uint32_t *) pixels) + y * width + x;
                 		    *pixel = color;
                 		}
                 	}
                 	if(pixels != 0){
                 	    pixels = 0;
                 	}
    	AndroidBitmap_unlockPixels(env, zBitmap);
}
jintArray getBitmap(JNIEnv *env,unsigned char * ptr){
    int len = WIDTH_CAMERA * HEIGHT_CAMERA * 3;
    jintArray a = env->NewIntArray(len);
    env->SetIntArrayRegion(a, 0, len, (jint*)ptr);
   //   __android_log_print(ANDROID_LOG_DEBUG, "opencv------->",  "ptr: %d",ptr);
    //  __android_log_print(ANDROID_LOG_DEBUG, "opencv------->",  "a: %d",a);
    return a;
}
extern "C" {
JNIEXPORT jobject JNICALL Java_com_iview_opencv_util_OpenCVHelper_detectPalmFromRawData
  (JNIEnv *env, jobject jthis, jobject bitmap, jint h, jint w, jstring path){
     unsigned char * ptrRGB = new unsigned char[WIDTH_CAMERA * HEIGHT_CAMERA * 3];
     extract(env, bitmap, ptrRGB);
     char* cStr=Jstring2CStr(env,path);
     int count=detectPalmFromRawData(ptrRGB,h,w,cStr);
    getResultBitmap(env, bitmap, ptrRGB);
     return bitmap;
  }

  JNIEXPORT jintArray JNICALL Java_com_iview_opencv_util_OpenCVHelper_gray
    (JNIEnv *env, jclass obj, jintArray buf, int w, int h){
        jint *cbuf;
        cbuf = env->GetIntArrayElements(buf, JNI_FALSE );
        if (cbuf == NULL) {
            return 0;
        }

        Mat imgData(h, w, CV_8UC4, (unsigned char *) cbuf);

        uchar* ptr = imgData.ptr(0);
        for(int i = 0; i < w*h; i ++){
            //计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
            //对于一个int四字节，其彩色值存储方式为：BGRA
            int grayScale = (int)(ptr[4*i+2]*0.299 + ptr[4*i+1]*0.587 + ptr[4*i+0]*0.114);
            ptr[4*i+1] = grayScale;
            ptr[4*i+2] = grayScale;
            ptr[4*i+0] = grayScale;
        }

        int size = w * h;
        jintArray result = env->NewIntArray(size);
        env->SetIntArrayRegion(result, 0, size, cbuf);
        env->ReleaseIntArrayElements(buf, cbuf, 0);
        return result;
    }

    JNIEXPORT jintArray JNICALL Java_com_iview_opencv_util_OpenCVHelper_getDealBitmap
      (JNIEnv *env, jclass jthis, jintArray buf, int w, int h){
              jint *cbuf;
              cbuf = env->GetIntArrayElements(buf, JNI_FALSE );
              if (cbuf == NULL) {
                  return 0;
              }
              Mat imgData(h, w, CV_8UC4, (unsigned char *) cbuf);
              unsigned char * ptr = imgData.ptr(0);
              char* cStr;
              int count=detectPalmFromRawData(ptr,h,w,cStr);
              int size = w * h;
              jintArray result = env->NewIntArray(size);
              env->SetIntArrayRegion(result, 0, size, cbuf);
              env->ReleaseIntArrayElements(buf, cbuf, 0);

             return result;
      }
}


