#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const Scalar BLUE(255, 0, 0);
const Scalar PURPLE(255, 0, 255);
const Scalar YELLOW(0 , 255, 255);
const Scalar COLOR[3] = {{255, 0, 0}, {0, 255, 0}, {0, 255, 255}};

int sigmax = 0;
Mat imgDraw;
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;

//void onBarChange(int value, void* data) {
//    Mat img;
//    cout << "check   " << sigmax << endl;
//    cvtColor(src, img, COLOR_BGR2GRAY);
//    GaussianBlur(img, img, Size(15, 15), (double)sigmax / 20, 0.0);
//    Canny(img, img, 22.0, 30.0);
//    imshow("demo", img);
//}

void drawCont(int index, int depth) {

//    cout << contourArea(contours[index]) << endl;

    vector<Point> contourHull;
    convexHull(contours[index], contourHull);
    double contArea = cv::contourArea(contours[index]);
    double hullArea = contourArea(contourHull);

//    cout << hullArea / contArea << endl;

    if (hullArea / contArea > 1.4) {
        drawContours(imgDraw, contours, index, PURPLE, 1);
        return;
    }
    drawContours(imgDraw, contours, index, COLOR[depth % 3], 1);
    for (int i = hierarchy[index][2]; i + 1; i = hierarchy[i][0]) {
        drawCont(i, depth + 1);
    }
}

void drawRect(int index) {
    vector<Point> contourHull;
    convexHull(contours[index], contourHull);
    double contArea = cv::contourArea(contours[index]);
    double hullArea = contourArea(contourHull);
    Scalar color;
    if (hullArea / contArea > 1.4) {
        color = YELLOW;
    } else {
        color = PURPLE;
    }
    RotatedRect rect =  minAreaRect(contours[index]);
    Point2f points[4];
    rect.points(points);
    for (int i = 0; i < 4; i++) {
        line(imgDraw, points[i], points[(i + 1) % 4], color, 2);
    }
}

int main() {
    VideoCapture cap("/home/snorlax/CLionProjects/HW_RM/hw1_2&3/video.mp4");
    Mat img;
    if (!cap.isOpened()) {
        cout << "Video not open!" << endl;
        exit(0);
    }

    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat channels[3];
    Mat red_sub_blue, imgThre;

    while (cap.read(img)) {
//        worse than put the blur below
//        GaussianBlur(img, img, Size(15, 15), 0., 0.);
        imgDraw = img.clone();
//        imgDraw = Mat::zeros(Size(img.cols, img.rows), CV_8UC3);

        split(img, channels);
        red_sub_blue = channels[2] - channels[0];
        normalize(red_sub_blue, red_sub_blue, 0., 255., NORM_MINMAX);
        GaussianBlur(red_sub_blue, red_sub_blue, Size(15, 15), 0.0, 0.0);
        threshold(red_sub_blue, imgThre, 100, 255, THRESH_OTSU);
//        morphologyEx(imgCanny, imgCanny, MORPH_DILATE, element);
//        morphologyEx(img, img, MORPH_CLOSE, element);

        findContours(imgThre, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE);
//        findContours(imgThre, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
        for (int i = 0; i + 1; i = hierarchy[i][0]) {
            if (contourArea(contours[i]) > 1000) {
                drawCont(i, 0);
                drawRect(i);
            }
        }

//        imshow("red_sub_blue", red_sub_blue);
        imshow("img", img);
        imshow("imgDraw", imgDraw);

        if (waitKey(40) >= 0) {
            exit(0);
        }
    }
//    waitKey(0);
    return 0;
}
