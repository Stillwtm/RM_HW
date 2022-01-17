#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

int thre, kernel_size;
cv::Mat src, draw;

void onSliderChange(int value, void* data) {
    cv::Mat src1 = (*(cv::Mat*)data).clone();
    cv::Mat res;
    cv::threshold(src1, res, thre, 255, cv::THRESH_BINARY);

    cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
    cv::morphologyEx(res, res, cv::MORPH_OPEN, element);

    cv::Rect pos = cv::boundingRect(res);
    draw = src.clone();
    cv::rectangle(draw, pos.tl(), pos.br(), {220, 20, 20}, 2);
    imshow("res", draw);
}

int main() {
    src = cv::imread("../apple.png");

    cv::Mat channels[3];
    cv::split(src, channels);
    cv::Mat red_sub_green = channels[2] - channels[1];

    cv::Mat norm_mat;
    cv::normalize(red_sub_green, norm_mat, 0., 255., cv::NORM_MINMAX);

    thre = 15;
    kernel_size = 31;
    namedWindow("bar", cv::WINDOW_NORMAL);
    cv::createTrackbar("threshold", "bar", &thre, 80,
                       onSliderChange, &norm_mat);
    cv::createTrackbar("kernel_size", "bar", &kernel_size, 81,
                       onSliderChange, &norm_mat);

    cv::waitKey(0);
    return 0;
}
