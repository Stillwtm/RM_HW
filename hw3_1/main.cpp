#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

float getDist(Point x, Point y) {
    return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y));
}

int main() {
    Mat src;
    src = imread("../data pack/car.jpg");
    // multi channels sub
    Mat channels[3];
    split(src, channels);
    Mat blue_sub_green = channels[0] - channels[1];
    normalize(blue_sub_green, blue_sub_green, 0., 255., NORM_MINMAX);
    // blur and threshold
    Mat img_thre;
    GaussianBlur(blue_sub_green, blue_sub_green, Size(15, 15), 0., 0.);
    threshold(blue_sub_green, img_thre, 60., 255., THRESH_BINARY);
    // close
    Mat kernel = getStructuringElement(MORPH_RECT, Size(31, 31));
    morphologyEx(img_thre, img_thre, MORPH_CLOSE, kernel);
    // find contours
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(img_thre, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // approx poly
    vector<vector<Point> > contours_poly(contours.size());
    Mat img_drawn = src.clone();
    for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) > 4000) {
            double peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], contours_poly[0], 0.02 * peri, true);
            drawContours(img_drawn, contours_poly, 0, Scalar(0, 255, 0), 2);
            for (int j = 0; j < 4; j++) {
                putText(img_drawn, to_string(j), contours_poly[0][j], FONT_HERSHEY_PLAIN,
                        2., Scalar(0, 255, 0), 2);
            }
        }
    }
    // perspective transformation
    contours_poly[0][3].x += 5; // do a small adjust by hand
    float w = getDist(contours_poly[0][0], contours_poly[0][3]);
    float h = getDist(contours_poly[0][0], contours_poly[0][1]);
    Point2f src_points[4] = {contours_poly[0][0], contours_poly[0][1],
                             contours_poly[0][2], contours_poly[0][3]};
    Point2f dst_points[4] = {{0, 0}, {0, h},
                             {w, h}, {w, 0}};
    Mat warp_matrix = getPerspectiveTransform(src_points, dst_points);
    Mat img_warp;
    warpPerspective(src, img_warp, warp_matrix, Size(w, h));
    // show image
    imshow("img_thre", img_thre);
    imshow("contour", img_drawn);
    imshow("warp", img_warp);
    waitKey(0);
    return 0;
}
