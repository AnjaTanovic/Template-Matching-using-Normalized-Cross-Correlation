#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "templatematching.hpp"

using namespace std;
using namespace cv;

//here are defined the locations of images and their templates
const int num_of_imgs = 20;
const string img_location[num_of_imgs] = { "../data/stars.jpg",
                                           "../data/stars_template.jpg", 
                                           "../data/coffee.jpg", 
                                           "../data/coffee_template.jpg", 
                                           "../data/pattern1.jpg", 
                                           "../data/pattern1_template.jpg", 
                                           "../data/diamonds.jpg", 
                                           "../data/diamonds_template.jpg", 
                                           "../data/pumpkins.jpg", 
                                           "../data/pumpkins_template.jpg", 
                                           "../data/river.jpg",
                                           "../data/river_template.jpg",
                                           "../data/ball.jpg",
                                           "../data/ball_template.jpg",
                                           "../data/pictures.jpg",
                                           "../data/pictures_template.jpg",
                                           "../data/logo.jpg",
                                           "../data/logo_template.jpg", 
                                           "../data/bubbles.jpg",
                                           "../data/bubbles_template.jpg"
                                            };

//temporary matrices, images are stored on one place for each iteration
Mat img; Mat templ; Mat result;

int match_method; //it is not necessary to define, CV_TM_CCOEFF_NORMED is always called

//temporary display matrix
Mat img_display;

//temporary matrix for locations of non-zero pixels on threshold
Mat locations;   // output, matrix has 1 col, elements are [x,y]

const char* image_window = "Source Image";
const char* thresh_window = "Threshold window";
const char* result_window = "Result window";

void MatchingMethod();

int main()
{
    for (int i = 0; i < num_of_imgs; i = i + 2)
    {
        img = imread(img_location[i], IMREAD_COLOR);
        templ = imread(img_location[i+1], IMREAD_COLOR);
        
        namedWindow(image_window, WINDOW_AUTOSIZE);
        namedWindow(thresh_window, WINDOW_AUTOSIZE);
        namedWindow(result_window, WINDOW_AUTOSIZE);

        MatchingMethod();
        waitKey(0);

    }

    return EXIT_SUCCESS;
}

void MatchingMethod()
{
    img.copyTo(img_display);

    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    match_method = TM_CCOEFF_NORMED;
    matchTemplate1(img, templ, result, match_method);

    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    Mat threshold = result >= 0.85;
    imshow(thresh_window, threshold);

    findNonZero(threshold, locations);

    int match_number = locations.rows; //number of matched pixels

    int i = 0;
    while (i < match_number)
    {
        rectangle(img_display, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + templ.cols, locations.at<Point>(i).y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift 0
        rectangle(result, Point(locations.at<Point>(i).x, locations.at<Point>(i).y), Point(locations.at<Point>(i).x + templ.cols, locations.at<Point>(i).y + templ.rows), Scalar(0, 0, 255), 2, 8, 0);  //color red, thickness 2, line type 8, shift 0
        i++;
    }


    imshow(image_window, img_display);
    imshow(result_window, result);

    return;
}

#include <opencv2/core/mat.hpp>
#include "opencv2/core/hal/hal.hpp"

using namespace std;
using namespace cv;

void matchTemplate1(InputArray _img, InputArray _templ, OutputArray _result, int method /*, InputArray _mask*/)
{
    //CV_INSTRUMENT_REGION();

    
    int type = _img.type(), depth = CV_MAT_DEPTH(type), cn = CV_MAT_CN(type);
    /*
    CV_Assert(CV_TM_SQDIFF <= method && method <= CV_TM_CCOEFF_NORMED);
    CV_Assert((depth == CV_8U || depth == CV_32F) && type == _templ.type() && _img.dims() <= 2);
    */


    /*
    if (!_mask.empty())
    {
        cv::matchTemplateMask(_img, _templ, _result, method, _mask);
        return;
    }
    */


    /*
    bool needswap = _img.size().height < _templ.size().height || _img.size().width < _templ.size().width;
    if (needswap)
    {
        CV_Assert(_img.size().height <= _templ.size().height && _img.size().width <= _templ.size().width);
    }
    */

    
    //CV_OCL_RUN(_img.dims() <= 2 && _result.isUMat(),
   //     (!needswap ? ocl_matchTemplate(_img, _templ, _result, method) : ocl_matchTemplate(_templ, _img, _result, method)))

    Mat img = _img.getMat(); Mat templ = _templ.getMat();
    
    /*
    if (needswap)
        std::swap(img, templ);
    */
     

    Size corrSize(img.cols - templ.cols + 1, img.rows - templ.rows + 1);
    _result.create(corrSize, CV_32F);
    Mat result = _result.getMat();

    //CV_IPP_RUN_FAST(ipp_matchTemplate1(img, templ, result, method)) proveriti
        //ipp_matchTemplate1(img, templ, result, method);
        crossCorr1(img, templ, result, Point(0, 0), 0, 0);

    common_matchTemplate1(img, templ, result, method, cn);
}

static void common_matchTemplate1(Mat& img, Mat& templ, Mat& result, int method, int cn)
{
    if (method == CV_TM_CCORR)
        return;

    int numType = method == CV_TM_CCORR || method == CV_TM_CCORR_NORMED ? 0 :
        method == CV_TM_CCOEFF || method == CV_TM_CCOEFF_NORMED ? 1 : 2;
    bool isNormed = method == CV_TM_CCORR_NORMED ||
        method == CV_TM_SQDIFF_NORMED ||
        method == CV_TM_CCOEFF_NORMED;

    double invArea = 1. / ((double)templ.rows * templ.cols);

    Mat sum, sqsum;
    Scalar templMean, templSdv;
    double* q0 = 0, * q1 = 0, * q2 = 0, * q3 = 0;
    double templNorm = 0, templSum2 = 0;

    if (method == CV_TM_CCOEFF)
    {
        integral(img, sum, CV_64F);
        templMean = mean(templ);
    }
    else
    {
        integral(img, sum, sqsum, CV_64F);
        meanStdDev(templ, templMean, templSdv);

        templNorm = templSdv[0] * templSdv[0] + templSdv[1] * templSdv[1] + templSdv[2] * templSdv[2] + templSdv[3] * templSdv[3];

        if (templNorm < DBL_EPSILON && method == CV_TM_CCOEFF_NORMED)
        {
            result = Scalar::all(1);
            return;
        }

        templSum2 = templNorm + templMean[0] * templMean[0] + templMean[1] * templMean[1] + templMean[2] * templMean[2] + templMean[3] * templMean[3];

        if (numType != 1)
        {
            templMean = Scalar::all(0);
            templNorm = templSum2;
        }

        templSum2 /= invArea;
        templNorm = std::sqrt(templNorm);
        templNorm /= std::sqrt(invArea); // care of accuracy here

        CV_Assert(sqsum.data != NULL);
        q0 = (double*)sqsum.data;
        q1 = q0 + templ.cols * cn;
        q2 = (double*)(sqsum.data + templ.rows * sqsum.step);
        q3 = q2 + templ.cols * cn;
    }

    CV_Assert(sum.data != NULL);
    double* p0 = (double*)sum.data;
    double* p1 = p0 + templ.cols * cn;
    double* p2 = (double*)(sum.data + templ.rows * sum.step);
    double* p3 = p2 + templ.cols * cn;

    int sumstep = sum.data ? (int)(sum.step / sizeof(double)) : 0;
    int sqstep = sqsum.data ? (int)(sqsum.step / sizeof(double)) : 0;

    int i, j, k;

    for (i = 0; i < result.rows; i++)
    {
        float* rrow = result.ptr<float>(i);
        int idx = i * sumstep;
        int idx2 = i * sqstep;

        for (j = 0; j < result.cols; j++, idx += cn, idx2 += cn)
        {
            double num = rrow[j], t;
            double wndMean2 = 0, wndSum2 = 0;

            if (numType == 1)
            {
                for (k = 0; k < cn; k++)
                {
                    t = p0[idx + k] - p1[idx + k] - p2[idx + k] + p3[idx + k];
                    wndMean2 += t * t;
                    num -= t * templMean[k];
                }

                wndMean2 *= invArea;
            }

            if (isNormed || numType == 2)
            {
                for (k = 0; k < cn; k++)
                {
                    t = q0[idx2 + k] - q1[idx2 + k] - q2[idx2 + k] + q3[idx2 + k];
                    wndSum2 += t;
                }

                if (numType == 2)
                {
                    num = wndSum2 - 2 * num + templSum2;
                    num = MAX(num, 0.);
                }
            }

            if (isNormed)
            {
                double diff2 = MAX(wndSum2 - wndMean2, 0);
                if (diff2 <= std::min(0.5, 10 * FLT_EPSILON * wndSum2))
                    t = 0; // avoid rounding errors
                else
                    t = std::sqrt(diff2) * templNorm;

                if (fabs(num) < t)
                    num /= t;
                else if (fabs(num) < t * 1.125)
                    num = num > 0 ? 1 : -1;
                else
                    num = method != CV_TM_SQDIFF_NORMED ? 0 : 1;
            }

            rrow[j] = (float)num;
        }
    }
}

void crossCorr1(const Mat& img, const Mat& _templ, Mat& corr, Point anchor, double delta, int borderType)
{
    const double blockScale = 4.5;
    const int minBlockSize = 256;
    std::vector<uchar> buf;

    Mat templ = _templ;
    int depth = img.depth(), cn = img.channels();
    int tdepth = templ.depth(), tcn = templ.channels();
    int cdepth = corr.depth(), ccn = corr.channels();

    CV_Assert(img.dims <= 2 && templ.dims <= 2 && corr.dims <= 2);

    if (depth != tdepth && tdepth != std::max(CV_32F, depth))
    {
        _templ.convertTo(templ, std::max(CV_32F, depth));
        tdepth = templ.depth();
    }

    CV_Assert(depth == tdepth || tdepth == CV_32F);
    CV_Assert(corr.rows <= img.rows + templ.rows - 1 &&
        corr.cols <= img.cols + templ.cols - 1);

    CV_Assert(ccn == 1 || delta == 0);

    int maxDepth = depth > CV_8S ? CV_64F : std::max(std::max(CV_32F, tdepth), cdepth);
    Size blocksize, dftsize;

    blocksize.width = cvRound(templ.cols * blockScale);
    blocksize.width = std::max(blocksize.width, minBlockSize - templ.cols + 1);
    blocksize.width = std::min(blocksize.width, corr.cols);
    blocksize.height = cvRound(templ.rows * blockScale);
    blocksize.height = std::max(blocksize.height, minBlockSize - templ.rows + 1);
    blocksize.height = std::min(blocksize.height, corr.rows);

    dftsize.width = std::max(getOptimalDFTSize(blocksize.width + templ.cols - 1), 2);
    dftsize.height = getOptimalDFTSize(blocksize.height + templ.rows - 1);
    if (dftsize.width <= 0 || dftsize.height <= 0)
    {
        cout << "the input arrays are too big" << endl;
        return;
    }

    // recompute block size
    blocksize.width = dftsize.width - templ.cols + 1;
    blocksize.width = MIN(blocksize.width, corr.cols);
    blocksize.height = dftsize.height - templ.rows + 1;
    blocksize.height = MIN(blocksize.height, corr.rows);

    Mat dftTempl(dftsize.height * tcn, dftsize.width, maxDepth);
    Mat dftImg(dftsize, maxDepth);

    int i, k, bufSize = 0;
    if (tcn > 1 && tdepth != maxDepth)
        bufSize = templ.cols * templ.rows * CV_ELEM_SIZE(tdepth);

    if (cn > 1 && depth != maxDepth)
        bufSize = std::max(bufSize, (blocksize.width + templ.cols - 1) *
            (blocksize.height + templ.rows - 1) * CV_ELEM_SIZE(depth));

    if ((ccn > 1 || cn > 1) && cdepth != maxDepth)
        bufSize = std::max(bufSize, blocksize.width * blocksize.height * CV_ELEM_SIZE(cdepth));

    buf.resize(bufSize);

    Ptr<hal::DFT2D> c = hal::DFT2D::create(dftsize.width, dftsize.height, dftTempl.depth(), 1, 1, CV_HAL_DFT_IS_INPLACE, templ.rows);

    // compute DFT of each template plane
    for (k = 0; k < tcn; k++)
    {
        int yofs = k * dftsize.height;
        Mat src = templ;
        Mat dst(dftTempl, Rect(0, yofs, dftsize.width, dftsize.height));
        Mat dst1(dftTempl, Rect(0, yofs, templ.cols, templ.rows));

        if (tcn > 1)
        {
            src = tdepth == maxDepth ? dst1 : Mat(templ.size(), tdepth, &buf[0]);
            int pairs[] = { k, 0 };
            mixChannels(&templ, 1, &src, 1, pairs, 1);
        }

        if (dst1.data != src.data)
            src.convertTo(dst1, dst1.depth());

        if (dst.cols > templ.cols)
        {
            Mat part(dst, Range(0, templ.rows), Range(templ.cols, dst.cols));
            part = Scalar::all(0);
        }
        c->apply(dst.data, (int)dst.step, dst.data, (int)dst.step);
    }

    int tileCountX = (corr.cols + blocksize.width - 1) / blocksize.width;
    int tileCountY = (corr.rows + blocksize.height - 1) / blocksize.height;
    int tileCount = tileCountX * tileCountY;

    Size wholeSize = img.size();
    Point roiofs(0, 0);
    Mat img0 = img;

    if (!(borderType & BORDER_ISOLATED))
    {
        img.locateROI(wholeSize, roiofs);
        img0.adjustROI(roiofs.y, wholeSize.height - img.rows - roiofs.y,
            roiofs.x, wholeSize.width - img.cols - roiofs.x);
    }
    borderType |= BORDER_ISOLATED;

    Ptr<hal::DFT2D> cF, cR;
    int f = CV_HAL_DFT_IS_INPLACE;
    int f_inv = f | CV_HAL_DFT_INVERSE | CV_HAL_DFT_SCALE;
    cF = hal::DFT2D::create(dftsize.width, dftsize.height, maxDepth, 1, 1, f, blocksize.height + templ.rows - 1);
    cR = hal::DFT2D::create(dftsize.width, dftsize.height, maxDepth, 1, 1, f_inv, blocksize.height);

    // calculate correlation by blocks
    for (i = 0; i < tileCount; i++)
    {
        int x = (i % tileCountX) * blocksize.width;
        int y = (i / tileCountX) * blocksize.height;

        Size bsz(std::min(blocksize.width, corr.cols - x),
            std::min(blocksize.height, corr.rows - y));
        Size dsz(bsz.width + templ.cols - 1, bsz.height + templ.rows - 1);
        int x0 = x - anchor.x + roiofs.x, y0 = y - anchor.y + roiofs.y;
        int x1 = std::max(0, x0), y1 = std::max(0, y0);
        int x2 = std::min(img0.cols, x0 + dsz.width);
        int y2 = std::min(img0.rows, y0 + dsz.height);
        Mat src0(img0, Range(y1, y2), Range(x1, x2));
        Mat dst(dftImg, Rect(0, 0, dsz.width, dsz.height));
        Mat dst1(dftImg, Rect(x1 - x0, y1 - y0, x2 - x1, y2 - y1));
        Mat cdst(corr, Rect(x, y, bsz.width, bsz.height));

        for (k = 0; k < cn; k++)
        {
            Mat src = src0;
            dftImg = Scalar::all(0);

            if (cn > 1)
            {
                src = depth == maxDepth ? dst1 : Mat(y2 - y1, x2 - x1, depth, &buf[0]);
                int pairs[] = { k, 0 };
                mixChannels(&src0, 1, &src, 1, pairs, 1);
            }

            if (dst1.data != src.data)
                src.convertTo(dst1, dst1.depth());

            if (x2 - x1 < dsz.width || y2 - y1 < dsz.height)
                copyMakeBorder(dst1, dst, y1 - y0, dst.rows - dst1.rows - (y1 - y0),
                    x1 - x0, dst.cols - dst1.cols - (x1 - x0), borderType);

            if (bsz.height == blocksize.height)
                cF->apply(dftImg.data, (int)dftImg.step, dftImg.data, (int)dftImg.step);
            else
                dft(dftImg, dftImg, 0, dsz.height);

            Mat dftTempl1(dftTempl, Rect(0, tcn > 1 ? k * dftsize.height : 0,
                dftsize.width, dftsize.height));
            mulSpectrums(dftImg, dftTempl1, dftImg, 0, true);

            if (bsz.height == blocksize.height)
                cR->apply(dftImg.data, (int)dftImg.step, dftImg.data, (int)dftImg.step);
            else
                dft(dftImg, dftImg, DFT_INVERSE + DFT_SCALE, bsz.height);

            src = dftImg(Rect(0, 0, bsz.width, bsz.height));

            if (ccn > 1)
            {
                if (cdepth != maxDepth)
                {
                    Mat plane(bsz, cdepth, &buf[0]);
                    src.convertTo(plane, cdepth, 1, delta);
                    src = plane;
                }
                int pairs[] = { 0, k };
                mixChannels(&src, 1, &cdst, 1, pairs, 1);
            }
            else
            {
                if (k == 0)
                    src.convertTo(cdst, cdepth, 1, delta);
                else
                {
                    if (maxDepth != cdepth)
                    {
                        Mat plane(bsz, cdepth, &buf[0]);
                        src.convertTo(plane, cdepth);
                        src = plane;
                    }
                    add(src, cdst, cdst);
                }
            }
        }
    }
}
