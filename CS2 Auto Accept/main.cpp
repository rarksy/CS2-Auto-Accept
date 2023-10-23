#include <Windows.h>
#include <opencv2/opencv.hpp>

bool TemplateMatch(cv::Mat Frame, cv::Mat ElementToFind, double Threshold, cv::Point& Detectedlocation)
{
    auto Result = cv::Mat(Frame.rows - ElementToFind.rows + 1, Frame.cols - ElementToFind.cols + 1, CV_32FC1);

    cv::matchTemplate(Frame, ElementToFind, Result, cv::TM_CCOEFF_NORMED);
    cv::threshold(Result, Result, Threshold, 1.0, cv::THRESH_TOZERO);
    double AccuracyValue;
    cv::minMaxLoc(Result, NULL, &AccuracyValue, NULL, &Detectedlocation);
    return AccuracyValue >= Threshold;
}

cv::Mat GetScreenshot(const cv::Rect& region, bool grayscale)
{
    // Get the width and height of the region
    int regionWidth = region.width;
    int regionHeight = region.height;

    // Get the device context for the desktop window
    auto hdc = GetDC(HWND_DESKTOP);

    // Create a bitmap compatible with the device context
    auto hbitmap = CreateCompatibleBitmap(hdc, regionWidth, regionHeight);

    // Create a memory device context compatible with the device context
    auto memdc = CreateCompatibleDC(hdc);

    // Select the bitmap into the memory device context
    auto oldbmp = SelectObject(memdc, hbitmap);

    // Copy the region of the screen specified by the cv::Rect object to the memory device context
    BitBlt(memdc, 0, 0, regionWidth, regionHeight, hdc, region.x, region.y, SRCCOPY);

    // Create a cv::Mat object to store the screenshot
    cv::Mat screenshot(regionHeight, regionWidth, CV_8UC4);

    // Get the bitmap data from the memory device context
    BITMAPINFOHEADER bi = {sizeof(bi), regionWidth, -regionHeight, 1, 32, BI_RGB};
    GetDIBits(hdc, hbitmap, 0, regionHeight, screenshot.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Clean up resources
    SelectObject(memdc, oldbmp);
    DeleteDC(memdc);
    DeleteObject(hbitmap);
    ReleaseDC(HWND_DESKTOP, hdc);

    // If grayscale is set to true, convert the screenshot to grayscale
    if (grayscale)
    {
        cv::Mat grayScreenshot;
        cv::cvtColor(screenshot, grayScreenshot, cv::COLOR_BGR2GRAY);
        return grayScreenshot;
    }

    return screenshot;
}

void ConvertTo1080p(cv::Mat& src) {
    cv::resize(src, src, cv::Size(), 1.0 / 1.3, 1.0 / 1.3, cv::INTER_AREA);
}

int main()
{
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN); 
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    cv::Rect ScreenSize(0, 0, ScreenWidth, ScreenHeight);
    
    cv::Mat AcceptResource = cv::imread("cs2_accept.png", cv::IMREAD_GRAYSCALE);
    if (ScreenHeight == 1080)
        ConvertTo1080p(AcceptResource);
    
    cv::Point DetectedLocation;

    std::cout << "CS2 Match Auto Accept By Ski" << std::endl;
    std::cout << "No, You Won't Get Banned." << std::endl;
    std::cout << "For More Info: https://discord.gg/vKjjS8yazu\n" << std::endl;
    std::cout << "Waiting For Match";
    std::cout.flush();
 
    // scan for accept button
    while (true)
    {
        cv::Mat ScreenFrame = GetScreenshot(ScreenSize, true);

        if (TemplateMatch(ScreenFrame, AcceptResource, 0.8, DetectedLocation))
            break;
        
        for (int i = 0; i < 3; i++)
        {
            std::cout << ".";
            std::cout.flush();
            Sleep(400);
        }
        std::cout << "\b\b\b   \b\b\b";
        Sleep(400);
    }

    // move mouse and click
    SetCursorPos(DetectedLocation.x + 100, DetectedLocation.y + 50);
    Sleep(100);
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
