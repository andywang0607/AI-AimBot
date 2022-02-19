#include "include/ScreenCaptureApi.h"

#include <chrono>
#include <thread>

#include "Windows.h"
#include "openpose/headers.hpp"
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

static ScreenCaptureApi *api = nullptr;

op::Wrapper opWrapper{op::ThreadManagerMode::Asynchronous};

int screenWidth = -1;
int screenHeight = -1;

bool isRecordResult = false;

std::string getTimeString()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    char time[32];
    std::strftime(time, sizeof(time), "%Y%m%d-%H%M%S", std::localtime(&now));

    return time;
}

bool processImage(cv::Mat &cvImageToProcess)
{
    auto datumsPtr = std::make_shared<std::vector<std::shared_ptr<op::Datum>>>();
    datumsPtr->emplace_back();
    auto &datumPtr = datumsPtr->at(0);
    datumPtr = std::make_shared<op::Datum>();

    try {
        datumPtr->cvInputData = OP_CV2OPCONSTMAT(cvImageToProcess);
        return opWrapper.tryEmplace(datumsPtr);

    } catch (const std::exception &e) {
        spdlog::info("exception: {}", e.what());
        return false;
    }
}

void showPersonNum(cv::Mat &mat, std::string &text)
{
    int baseline = 0;
    cv::Size textSize =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 2, 2, &baseline);

    cv::Point origin(0, textSize.height);

    cv::putText(mat, text, origin, cv::FONT_HERSHEY_SIMPLEX, 2,
                cv::Scalar(255, 0, 0));
}

class AimbotSpi : public ScreenCaptureSpi
{
    void onStreamRtn(unsigned char *data, int length) override
    {
        auto mat = cv::imdecode(std::vector<unsigned char>(data, data + length),
                                cv::IMREAD_COLOR);
        processImage(mat);
    }

    void onImageRtn(unsigned char *data, int length) override {}

    void onConnectRspRtn(int imgWidth, int imgHeight) override
    {
        screenWidth = imgWidth;
        screenHeight = imgHeight;
    }

    void onStartQueryScreenStreamRspRtn(const char *msg) override {}

    void onStopQueryScreenStreamRspRtn(const char *msg) override {}
    void onDisConnectRspRtn(const char *msg) override {}

    std::chrono::time_point<std::chrono::steady_clock> lastTime_ =
        std::chrono::steady_clock::now();

} mySpi;

using namespace std::literals::chrono_literals;

int main()
{
    api = ScreenCaptureApi::create(mySpi);

    api->connect("127.0.0.1", 8080);
    api->startQueryScreenStream();

    op::WrapperStructPose structPose;

    // Default netInputSize: Point<int>{-1, 368}
    structPose.netInputSize = op::Point<int>{-1, 352};
    opWrapper.configure(structPose);
    opWrapper.setDefaultMaxSizeQueues(1);
    opWrapper.disableMultiThreading();
    opWrapper.start();

    std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>> result;

    while(screenWidth == -1 && screenHeight == -1) {
        std::this_thread::sleep_for(100ms);
    }

    cv::VideoWriter out;
    if (isRecordResult) {
        std::string outFileName = getTimeString() + ".mpg";
        auto ret = out.open(
            outFileName,
            CV_FOURCC('D', 'I', 'V', 'X'),
            10.0,
            cv::Size(screenWidth, screenHeight),
            true);
    }

    while (true) {
        if (!opWrapper.waitAndPop(result)) {
            spdlog::info("waitAndPop failed");
        }
        cv::Mat cvMat = OP_OP2CVCONSTMAT(result->at(0)->cvOutputData);
        if (cvMat.empty()) {
            continue;
        }
        int personNum = result->at(0)->poseKeypoints.getSize(0);
        showPersonNum(cvMat, std::to_string(personNum));
        if (personNum > 0) {
            int personIdx = 0;
            const auto x = result->at(0)->poseKeypoints[{personIdx, 0, 0}];
            const auto y = result->at(0)->poseKeypoints[{personIdx, 0, 1}];
            const auto score = result->at(0)->poseKeypoints[{personIdx, 0, 2}];

            auto point = cv::Point2f(x, y);
            cv::circle(cvMat, point, 50, cv::Scalar(0, 0, 255));

            const static auto altKey =  0x8000;
            if (GetKeyState(VK_MENU) & altKey) {
                SetCursorPos(x, y);
            }
        }
        if (isRecordResult) {
            out << cvMat;
        }
        cv::imshow("OpenPoseResult", cvMat);
        
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    api->stopQueryScreenStream();
    api->disconnect();
    using namespace std::literals::chrono_literals;
    std::this_thread::sleep_for(3s);
    return 0;
}