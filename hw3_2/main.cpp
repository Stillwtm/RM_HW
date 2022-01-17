#include <Eigen/Dense>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>
#include <iostream>

using namespace std;
using namespace cv;

constexpr int IMG_NUM = 1;
constexpr int BOARD_W = 9;
constexpr int BOARD_H = 6;
constexpr int BOARD_N = BOARD_W * BOARD_H;
const Size BOARD_SIZE(BOARD_W, BOARD_H);

int main() {
    Mat camera_matrix = (Mat_<float>(3, 3) <<
            9.1234465679824348e+02, 0., 6.4157634413436961e+02,
            0., 7.6573154962089018e+02, 3.6477945186797331e+02,
            0., 0., 1.);
    Mat dist_coeffs = (Mat_<double>(5, 1) <<
            0., -4.2669718747763807e-01, 2.6509688616309912e-01,
            -5.3624979910268683e-04, -4.1011485564833132e-04);

    for (int i = 0; i < IMG_NUM; i++) {
        string filename = "../data pack/chess/" + to_string(i) + ".jpg";
        Mat src = imread(filename);

        vector<Point2f> corners;
        bool found = findChessboardCorners(src, BOARD_SIZE, corners);
        if (!found || corners.size() != BOARD_N) {
            cout << "img " << i << " failed!" << endl;
            continue;
        }
        Mat img_gray;
        cvtColor(src, img_gray, COLOR_BGR2GRAY);
        find4QuadCornerSubpix(img_gray, corners, Size(5, 5));

        vector<Point3f> obj_points;
        for (int i = 0; i < BOARD_N; i++) {
            obj_points.emplace_back(Point3f((float) (i / BOARD_W), (float) (i % BOARD_W), 0.f));
        }

        Mat_<float> rvec, tvec;
//        only use four points
//        vector<Point3f> obj = {obj_points[0], obj_points[8], obj_points[BOARD_N - 9], obj_points[BOARD_N - 1]};
//        vector<Point2f> cor = {corners[0], corners[8], corners[BOARD_N - 9], corners[BOARD_N - 1]};
//        solvePnP(obj, cor, camera_matrix, dist_coeffs, rvec, tvec);
        // use all points
        solvePnP(obj_points, corners, camera_matrix, dist_coeffs, rvec, tvec);

        cout << "rvec:" << endl << rvec << endl;
        cout << "tvec:" << endl << tvec << endl;

        Mat_<float> rMat(3, 3);
        Rodrigues(rvec, rMat);

        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> R_n;
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> T_n;
        cv2eigen(rMat, R_n);
        cv2eigen(tvec, T_n);
        Eigen::Vector3f P_oc;
        P_oc = -R_n.inverse() * T_n;

        cout << "img " << i << endl << "P_oc:" << endl << P_oc << endl;

//        waitKey(0);
    }
    return 0;
}
