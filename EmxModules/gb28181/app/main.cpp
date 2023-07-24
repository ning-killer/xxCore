#include "DeviceConfig.h"
#include "SipUAC.h"
#include "VideoHandler.h"

void CreateShutdownScript();

int main(int argc, char* argv[])
{
//	spdlog::init_thread_pool(3096, 1);
//	auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
//	stdout_sink->set_level(spdlog::level::debug);
//	stdout_sink->set_color(spdlog::level::debug, stdout_sink->yellow);
//	stdout_sink->set_color(spdlog::level::info, stdout_sink->green);
//	stdout_sink->set_color_mode(spdlog::color_mode::automatic);
//	stdout_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%s:%#][%t][%l] %v");
//
//	auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("log/all.txt", 1024 * 1024 * 8, 3);
//	rotating_sink->set_level(spdlog::level::debug);
//	rotating_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%s:%#][%t][%l] %v");
//
//	std::vector<spdlog::sink_ptr> sinks{ stdout_sink, rotating_sink };
//	auto logger = std::make_shared<spdlog::async_logger>("all", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
//	logger->set_level(spdlog::level::debug);
//	spdlog::set_default_logger(logger);

	DeviceConfig::Instance().Load("config.json");
	SipUAC::Instance().Init();

	CreateShutdownScript();

	while (true)
	{
		CheckHandlerTimeout();
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

//	SPDLOG_ERROR("quit application");
//	spdlog::shutdown();
	return 0;
}

void CreateShutdownScript()
{
//	int fd = open("shutdown.sh", O_CREAT | O_WRONLY | O_TRUNC, 0777);
//	if (fd == -1)
//	{
//		SPDLOG_ERROR("create shutdown script failed");
//		return;
//	}
//	char tmp[32];
//	auto pid = getpid();
//	int sz = sprintf(tmp, "kill -9 %d", pid);
//	write(fd, tmp, sz);
//	close(fd);
}