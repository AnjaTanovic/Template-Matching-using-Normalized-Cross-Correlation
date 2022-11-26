#pragma once

#ifndef TEMPLATEMATCHING_HPP_
#define TEMPLATEMATCHING_HPP_

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/core/hal/hal.hpp"


//Method: 0: SQDIFF 1: SQDIFF NORMED 2: TM CCORR 3: TM CCORR NORMED 4: TM COEFF 5: TM COEFF NORMED
#define CV_TM_SQDIFF 0
#define CV_TM_SQDIFF_NORMED 1
#define CV_TM_CCORR 2
#define CV_TM_CCORR_NORMED 3
#define CV_TM_CCOEFF 4
#define CV_TM_CCOEFF_NORMED 5

using namespace std;
using namespace cv;

void matchTemplate1(InputArray _img, InputArray _templ, OutputArray _result, int method /*, InputArray _mask*/);

//static bool ipp_matchTemplate1(Mat& img, Mat& templ, Mat& result, int method);  //moze se napraviti i kao void

//static bool ipp_crossCorr1(const Mat& src, const Mat& tpl, Mat& dst, bool normed);

static void common_matchTemplate1(Mat& img, Mat& templ, Mat& result, int method, int cn);

void crossCorr1(const Mat& img, const Mat& _templ, Mat& corr, Point anchor, double delta, int borderType);

#endif