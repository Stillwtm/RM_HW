#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

constexpr int IMG_NUM = 24;
constexpr int BOARD_W = 9;
constexpr int BOARD_H = 6;
constexpr int BOARD_N = BOARD_W * BOARD_H;
const Size BOARD_SIZE(BOARD_W, BOARD_H);

int main() {
    Mat src, img_drawn;
    bool found;
    int success_cnt = 0;
    Size img_size;
    vector<Point2f> corners;
    vector<vector<Point2f> > img_points;
    vector<vector<Point3f> > obj_points;
    for (int i = 0; i < IMG_NUM; i++) {
        string filename = "../calib2/" + to_string(i) + "_orig.jpg";
        src = imread(filename);
        found = findChessboardCorners(src, BOARD_SIZE, corners);
        if (found && corners.size() == BOARD_N) {
            success_cnt++;
            img_size = src.size();
//            draw found corners
//            img_drawn = src.clone();
//            drawChessboardCorners(img_drawn, BOARD_SIZE, corners, found);
//            imshow("img_drawn", img_drawn);

            cvtColor(src, src, COLOR_BGR2GRAY);
            find4QuadCornerSubpix(src, corners, Size(5, 5));

            // don't know how the code below actually works
//            TermCriteria criteria = cv::TermCriteria(
//                    cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
//                    40,
//                    0.01);
//            cornerSubPix(src, corners, Size(5, 5), Size(-1, -1), criteria);
//            cornerSubPix(src, corners, Size(5, 5));

            // write in the coordinate
            img_points.emplace_back(corners);
            obj_points.emplace_back(vector<Point3f>());
            vector<Point3f>& opt = obj_points.back();
            for (int j = 0; j < BOARD_N; j++) {
                opt.emplace_back(Point3f((float)(j / BOARD_W), (float)(j % BOARD_W), 0.f));
            }
        } else {
            cout << "found " << corners.size() << " corners!" << endl;
            cout << "number: " << i << "  failed!" << endl;
        }
    }

    cout << success_cnt << " useful boards" << endl;

    Mat camera_matrix, dist_coeffs, rvecs, tvecs;
    calibrateCamera(obj_points, img_points, img_size,camera_matrix,
                    dist_coeffs, rvecs, tvecs);
    cout << "camera_matrix: " << endl << camera_matrix << endl;
    cout << "dist_coeffs: " << endl << dist_coeffs << endl;
    return 0;
}
