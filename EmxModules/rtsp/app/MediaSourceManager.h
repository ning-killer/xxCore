#pragma once
#include <memory>
#include <map>
#include <string>

class MediaSource;

class MediaSourceManager :public std::enable_shared_from_this<MediaSourceManager>
{
public:
	~MediaSourceManager();

	static MediaSourceManager& Instance();

	void addMedia(const std::string& app, std::shared_ptr<MediaSource> src);

	std::shared_ptr<MediaSource> searchMedia(const std::string& app);

	void removeAllMedia();
private:
	MediaSourceManager() = default;
private:
	std::map<std::string, std::shared_ptr<MediaSource>> _mediaMap;
};

