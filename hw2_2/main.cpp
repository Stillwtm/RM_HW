#include "CameraApi.h" //相机SDK的API头文件
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

constexpr int IMG_NUM = 30;
constexpr int BOARD_W = 11;
constexpr int BOARD_H = 8;
constexpr int BOARD_N = BOARD_W * BOARD_H;
const Size BOARD_SIZE(BOARD_W, BOARD_H);

unsigned char *g_pRgbBuffer;     //处理后数据缓存区

bool findCorners(Mat& img, vector<Point2f>& corners) {
    bool found = findChessboardCorners(img, BOARD_SIZE, corners);
    if (found && corners.size() == BOARD_N) {
        Mat img_gray;
        cvtColor(img, img_gray, COLOR_BGR2GRAY);
        find4QuadCornerSubpix(img_gray, corners, Size(5, 5));
        return true;
    } else {
        return false;
    }
}

int main() {
    int iCameraCounts = 1;
    int iStatus = -1;
    tSdkCameraDevInfo tCameraEnumList;
    int hCamera;
    tSdkCameraCapbility tCapability;      //设备描述信息
    tSdkFrameHead sFrameInfo;
    BYTE *pbyBuffer;
    int iDisplayFrames = 10000;
    int channel = 3;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);
    printf("state = %d\n", iStatus);

    printf("count = %d\n", iCameraCounts);
    //没有连接设备
    if (iCameraCounts == 0) {
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);

    //初始化失败
    printf("state = %d\n", iStatus);
    if (iStatus != CAMERA_STATUS_SUCCESS) {
        return -1;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera, &tCapability);

    //
    g_pRgbBuffer = (unsigned char *) malloc(
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3);
    //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(hCamera);

    /*其他的相机参数设置
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
         CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
         CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
         更多的参数的设置方法，，清参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    */

    if (tCapability.sIspCapacity.bMonoSensor) {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } else {
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
    }

    int success_cnt = 0;
    double last_captured_stamp = 0;
    vector<Point2f> corners;
    vector<vector<Point2f> > img_points;
    vector<vector<Point3f> > obj_points;
    Size img_size;

    // do until succeed IMG_NUM images
    while (success_cnt < IMG_NUM) {
        if (CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS) {
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);

            cv::Mat img(
                    Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
                    sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,
                    g_pRgbBuffer
            );
            img_size = img.size();
            Mat img_show;
            resize(img, img_show, Size(640, 320));
            imshow("Opencv Demo", img_show);

            bool found = findCorners(img, corners);

            double timestamp = clock() / CLOCKS_PER_SEC;
            if (found && timestamp > last_captured_stamp + 1) {
                last_captured_stamp = timestamp;
                success_cnt++;
                // put coordinates in
                img_points.emplace_back(corners);
                obj_points.emplace_back(vector<Point3f>());
                vector<Point3f>& opt = obj_points.back();
                for (int j = 0; j < BOARD_N; j++) {
                    opt.emplace_back(Point3f((float)(j / BOARD_W), (float)(j % BOARD_W), 0.f));
                }

                cout << "image: " << success_cnt << "succeed!" << endl;
            }

            int key = waitKey(5);

            //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
            //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
            CameraReleaseImageBuffer(hCamera, pbyBuffer);

            if (key == 'q') {
                break;
            }
        }
    }

    Mat camera_matrix, dist_coeffs;
    calibrateCamera(obj_points, img_points, img_size,
                    camera_matrix, dist_coeffs, noArray(), noArray());

    cout << "camera_matrix: " << endl << camera_matrix << endl;
    cout << "dist_coeffs: " << endl << dist_coeffs << endl;

    CameraUnInit(hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);

    return 0;
}
