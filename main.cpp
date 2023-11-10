#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string.h>

const double	HOUGH_DELTA_RHO     = 5;
const double	HOUGH_DELTA_THETA	= CV_PI / 180.;
const int		HOUGH_MIN_VOTE		= 1;
const double	HOUGH_LENGTH		= 1.;
const double	HOUGH_GAP			= 80.;

    struct CHoughTFInterface
    {
        int		mDeltaRho           = HOUGH_DELTA_RHO;					// The resolution of rho the parameter r in pixels.We use 1 pixel.
        int		mMAX_DELTA_RHO      = 100;
        double  rhoRatio            = 0.01;

        int		mcDeltaThetaRatio       = 60;			// 누산기 해상도 파라미터에 적용할 배수
        int		mcDELTA_THETA_RATIO     = 180;		// 누산기 해상도 파라미터에 적용할 배수의 최대값
        double	mDeltaTheta             = CV_PI / 180.0;				// 누산기 해상도 파라미터

        int		mMinVote                = 1;					// 선을 고려하기 전에 받아야 하는 최소 투표 개수
        int		mcMAXVOTE               = 100;

        int		mcLengthRatio           = 1;				// 선에 대한 최소 길이 비율 파라메터
        int		mcMAXLENGTH             = 1000;
        double	mMinLength              = 1.0;					// 선에 대한 최소 길이

        int		mcGapRatio              = HOUGH_GAP;//200;					// 선에 따른 최대 허용 간격 파라메터
        int		mcMAX_GAP_RATIO         = 200;			// 선에 따른 최대 허용 간격 파라메터의 최대값
        double	mMaxGap = 1.0;				// 선에 따른 최대 허용 간격

        cv::Mat calculateP(cv::Mat& src)
        {
            /// input 영상은 Gray 영상만 해야함
            if (mDeltaRho == 0)
                mDeltaRho = 1;

            if (mcDeltaThetaRatio == 0)
                mcDeltaThetaRatio = 1;

            if (mMinVote == 0)
                mMinVote = 1;

            // 원본 이미지에 덮어 보여주는 것이므로 백업용 이미지를 가지고 있어야 함
            // cv::Mat dst = cv::Mat(src.size(), CV_32FC2);
            cv::Mat dst;
            cv::cvtColor(src, dst, cv::COLOR_GRAY2RGB);

            /// Canny의 result를 가져다 씀
            {
                rhoRatio = 0.01;
                std::vector<cv::Vec4i> lines; // for HoughLinesP
                cv::HoughLinesP(src, lines, mDeltaRho * rhoRatio, mDeltaTheta * mcDeltaThetaRatio, mMinVote, mcLengthRatio * mMinLength, mcGapRatio* mMaxGap);

                for (auto& l : lines)
                {
                    cv::line(dst, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 255), 3, cv::LINE_AA);	/// BGR 순임
                }
            }
            return dst;
        }
        
        cv::Mat calculate(cv::Mat& src)
        {
            /// input 영상은 Gray 영상만 해야함
            if (mDeltaRho == 0)
                mDeltaRho = 1;

            if (mcDeltaThetaRatio == 0)
                mcDeltaThetaRatio = 1;

            if (mMinVote == 0)
                mMinVote = 1;

            // 원본 이미지에 덮어 보여주는 것이므로 백업용 이미지를 가지고 있어야 함
            // cv::Mat dst = cv::Mat(src.size(), CV_32FC2);
            cv::Mat dst;
            cv::cvtColor(src, dst, cv::COLOR_GRAY2RGB);
            // hough lines
            {
                rhoRatio = 1;
                std::vector<cv::Vec2f> lines; // for HoughLines    
                cv::HoughLines(src, lines, mDeltaRho * rhoRatio, mDeltaTheta * mcDeltaThetaRatio, mMinVote);
                for (auto& l : lines)
                {
                    float rho = l[0];
                    float theta = l[1];

                    cv::Point pt1, pt2;
                    double a = cos(theta), b = sin(theta);
                    double x0 = a*rho, y0 = b*rho;
                    pt1.x = cvRound(x0 + 1000*(-b));
                    pt1.y = cvRound(y0 + 1000*(a));
                    pt2.x = cvRound(x0 - 1000*(-b));
                    pt2.y = cvRound(y0 - 1000*(a));
                    cv::line(dst, pt1, pt2, cv::Scalar(0, 255, 255), 3, cv::LINE_AA);
                }
            }

            return dst;
        }
    };

    std::string _mTrackbarName  = "houghLines";
    std::string trackbarNameP   = "houghLinesP";

	void setHoughTransform(int value, void* data)
	{
		// CHoughTFInterface* h = (CHoughTFInterface*)data;
        int* h = (int*)data;
        *h = value;
	}

int main(int argc, char** argv) {
    cv::Mat image = cv::imread("/Users/junsang/workspace/obsidian/4.Archive/99.Attached.file/120_119_density_map.jpg");//argv[0]);
    if (image.empty()) {
        std::cerr << "Failed to load image" << std::endl;
        return -1;
    }
    CHoughTFInterface* data = new CHoughTFInterface();
    cv::Mat src;
    cv::cvtColor(image, src, cv::COLOR_RGB2GRAY);
    // cv::cvtColor(image, src, cv::COLOR_RGBA2RGB);

    // 윈도우 생성
    cv::namedWindow(_mTrackbarName, cv::WINDOW_NORMAL);

    // // 초기 밝기 설정
    cv::createTrackbar("dist from '0'",         _mTrackbarName, &data->mDeltaRho,           data->mMAX_DELTA_RHO,		setHoughTransform, &data->mDeltaRho);
    cv::setTrackbarPos("dist from '0'",         _mTrackbarName, data->mDeltaRho);

    cv::createTrackbar("Theta",	_mTrackbarName, &data->mcDeltaThetaRatio,   data->mcDELTA_THETA_RATIO,	setHoughTransform, data);
    cv::createTrackbar("threshold pts",			_mTrackbarName, &data->mMinVote,            data->mcMAXVOTE,			setHoughTransform, data);
    // cv::createTrackbar("Min length",		_mTrackbarName, &data->mcLengthRatio,       data->mcMAXLENGTH,			setHoughTransform, data);
    // cv::createTrackbar("Max Gap",			_mTrackbarName, &data->mcGapRatio,          data->mcMAX_GAP_RATIO,		setHoughTransform, data);


    CHoughTFInterface dataP;
    // 윈도우 생성
    cv::namedWindow(trackbarNameP, cv::WINDOW_NORMAL);

    // // 초기 밝기 설정
    cv::createTrackbar("dist from '0'",         trackbarNameP, &dataP.mDeltaRho,           data->mMAX_DELTA_RHO,		setHoughTransform, &dataP);
    cv::setTrackbarPos("dist from '0'",         trackbarNameP, dataP.mDeltaRho);

    cv::createTrackbar("Theta",	trackbarNameP, &dataP.mcDeltaThetaRatio,   data->mcDELTA_THETA_RATIO,	setHoughTransform, &dataP);
    cv::createTrackbar("threshold pts",			trackbarNameP, &dataP.mMinVote,            data->mcMAXVOTE,			    setHoughTransform, &dataP);
    cv::createTrackbar("Min line l:",		trackbarNameP, &dataP.mcLengthRatio,       data->mcMAXLENGTH,			setHoughTransform, &dataP);
    cv::createTrackbar("Max line Gap",			trackbarNameP, &dataP.mcGapRatio,          data->mcMAX_GAP_RATIO,		setHoughTransform, &dataP);

    while(true)
    {
        cv::Mat rst = data->calculate(src);
        cv::imshow(_mTrackbarName, rst);

        cv::Mat rst2 = dataP.calculateP(src);
        cv::imshow(trackbarNameP, rst2);

        char key = cv::waitKey(30);
        if (key == 27)
            break;
    }

    delete data;
    return 0;
}
