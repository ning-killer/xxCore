【demo编译步骤】
1. demo入口函数为cmcc_rtc_demo.cc的main()，将结构体变量login_params中的字段更改为所申请到的账号信息
2. 按需配置交叉编译工具链和修改Makefile
3. 在当前目录执行make，即可生成可执行文件cmcc_rtc_demo。


【demo功能说明】
1. 这个demo程序用来实现拨打电话、接听电话、挂断电话等功能。其中，用文件读写来模拟音频录制发送和接收，用读文件模拟读取编码后的视频帧数据。

2.音频输入和输出文件名定义在third_party_device_factory.cc中，放置在程序运行目录下：
char ThirdpartyAudioDeviceFactory::_inputAudioFilename[MAX_FILENAME_LEN] = "input16000.pcm";
char ThirdpartyAudioDeviceFactory::_outputAudioFilename[MAX_FILENAME_LEN] = "output16000.pcm";

3.视频发送文件放在send/目录下。

4.适配时把third_party_device.cc中的RecThreadProcess()读文件处理和PlayThreadProcess()写文件改写为实际的音频采集和播放即可实现音频的采集发送和接收播放；
仿照demo中的send_video_thread()读视频文件，进行读取实际的编码后视频帧，即可实现视频的发送。