#include <afxwin.h>      //MFC core and standard component
#include <atlimage.h>    //included for CImage
#include <afxdlgs.h>
#include <afxcmn.h>
#include "resource1.h"   //main symbols
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdint.h>
#include <fstream>

class MatrixCalculations
{
  public:
    //calculates margins, cutting left columns from left image, and right columns from right image
    static void marginCalculation(int &total_compatibility, int &total_compatibility_cols_taken, cv::Mat &img_total_output1, cv::Mat &img_total_output2, cv::Mat img_first, cv::Mat img_second);
    //helper to see the shift matrix located at D:/exmaple.txt
    static void helperLogger(int ** x, int len);
    //Correlation calculation, cut into boxes left image, and check for best compatiblity on the right image horizontally
    static void calcCorrelation(int **shift_matrix,cv::Mat RightSrc, cv::Mat leftC , int len, int marg_len, CProgressCtrl *prog);
    // same us up, but skips if square is totally white
    static void calcCorrelationSkip(int **shift_matrix, cv::Mat RightSrc, cv::Mat leftC, int len, int marg_len, CProgressCtrl *prog);
    //Inner loop of calcCorrelation
    static void squareCorrelationCheck(cv::Mat src, cv::Mat letC, int &best_correlation, int **shift_matrix, int x, int y, int len, int marg_len, int shift);
    //Set of functions, that mirror parts of image
    static bool AreTherePixelsOtherThanWhite(cv::Mat m, int x, int y, int marg_len);
    static void depthOfFieldCalculation(int **shift_matrix, float parameter_f, float parameter_B, int size);
    static void MatrixCalculations::mirrorMargins(cv::Mat &matrix, cv::Mat matrix_inside, cv::Size box);
    static void MatrixCalculations::helperLoggerF(float ** x, int len);
    static void downRightCornerMirror(cv::Mat &matrix, cv::Size box);
    static void downLeftCornerMirror(cv::Mat &matrix, cv::Size box);
    static void downMidMirror(cv::Mat &matrix, cv::Size box);
    static void midLeftMirror(cv::Mat &matrix, cv::Size box);
    static void midRightMirror(cv::Mat &matrix, cv::Size box);
    static void topRightCornerMirror(cv::Mat &matrix, cv::Size box);
    static void topLeftCornerMirror(cv::Mat &matrix, cv::Size box);
    static void topMidMirror(cv::Mat &matrix, cv::Size box);
    

};

