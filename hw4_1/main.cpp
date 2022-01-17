#include <Eigen/Dense>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/eigen.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    Mat depth = imread("../data pack/1_dpt.tiff", IMREAD_ANYDEPTH);
    Mat img = imread("../data pack/1.jpg");
    FileStorage reader("../data pack/data.yml", FileStorage::READ);
    Mat camera_matrix, pMat;
    reader["C"] >> camera_matrix;
    reader["D"] >> pMat;

//    Mat img_gray;
//    cvtColor(img, img_gray, COLOR_BGR2GRAY);
//    // create orb detector
//    Ptr<ORB> orb = ORB::create();
//    // detect key points
//    vector<KeyPoint> feature_points;
//    orb->detect(img_gray, feature_points);

    /* temporarily ignore this
    vector<>
    undistortPoints()
    */

    // draw feature points
//    Mat canvas;
//    drawKeypoints(img, feature_points, canvas);
//    imshow("key_points", canvas);
//    waitKey(0);

    Mat_<double> R(3, 3);
    Mat_<double> tVec(3, 1), rVec(3, 1);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R.at<double>(i, j) = pMat.at<double>(i, j);
    Rodrigues(R, rVec);
    tVec << pMat.at<double>(0, 3),
            pMat.at<double>(1, 3),
            pMat.at<double>(2, 3);
//    cout << pMat << endl << rVec << endl << tVec << endl;
    // create a img coordinate ((x, y, 1)?)
    vector<Point2f> img_points, img_points_norm;
    for (int i = 0; i < img.rows; i++)
        for (int j = 0; j < img.cols; j++)
            img_points.emplace_back(j, i);

    // like kind of normalize?
    undistortPoints(img_points, img_points_norm, camera_matrix, noArray());
//    cout << img_points_norm << endl;
    // create coordinate 3D based on depth
    vector<Point3f> obj_points;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            int index = i * img.cols + j;
            float d = depth.at<float>(i, j);
            obj_points.emplace_back(d * img_points_norm[index].x,
                                    d * img_points_norm[index].y,
                                    d);
        }
    }
    // why can the code below work as well as the code above ???
//    for (auto& pt : img_points_norm) {
//        float d = depth.at<float>(pt.y, pt.x);
//        obj_points.emplace_back(d * pt.x, d * pt.y, d);
//    }

    // project 3D coordinate to 2D
    vector<Point2f> res_points;
    projectPoints(obj_points, rVec, tVec, camera_matrix,
                  noArray(), res_points);

    // draw result
    Mat canvas = Mat::zeros(3 * img.rows, 3 * img.cols, CV_8UC3);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            int index = i * img.cols + j;
//            int r = cvRound(res_points[index].y);
//            int c = cvRound(res_points[index].x);
            int r = cvRound(res_points[index].y) + img.rows;
            int c = cvRound(res_points[index].x) + img.cols;
            if (0 <= r && r < 3 * img.rows && 0 <= c && c < 3 * img.cols)
                canvas.at<Vec3b>(r, c)= img.at<Vec3b>(i, j);
        }
    }

    // display result
    resize(canvas, canvas, Size(), 0.2, 0.2);
    imshow("res", canvas);
    waitKey(0);

    return 0;
}
