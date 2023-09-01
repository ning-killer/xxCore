/**
 * @file   DemoQRScan.cpp
 * @author hening
 * @brief  媒体服务二维码业务单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

class DemoQrScan {
  public:
    DemoQrScan() : m_qr(0) { }
    void Create(EuvLoop &loop, char *buffer, int bufferSize) {
        // 启动扫码，扫码需要一个buffer用来接收数据
        // 注册一个回调函数，当扫描到有效的二维码后触发
        m_qr.StartQR(loop, buffer, bufferSize, [](int chn, const char *result) {
            emxlogd("chn %d get QR content#%s#\n", chn, result);
        });
    }

    void Destroy() {
        //停止扫码
        m_qr.StopQR();
    }

  private:
    MediaClientQRScan m_qr; // 定义一个二维码扫描对象用来执行扫码
};

int main(int argc, char *argv[]) {
    DemoQrScan qrDemo;
    EuvLoop loop;
    char buffer[1024];
    loop.Init("DemoQRScan", []() { });
    qrDemo.Create(loop, buffer, 1024);
    emxlogd("DemoQRScan loop start.\n");
    loop.Start(false);
    return 0;
}