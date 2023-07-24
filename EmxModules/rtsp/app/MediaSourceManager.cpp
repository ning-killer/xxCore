#include "MediaSourceManager.h"

MediaSourceManager::~MediaSourceManager()
{}

MediaSourceManager& MediaSourceManager::Instance()
{
	static std::shared_ptr<MediaSourceManager> g_instance(new MediaSourceManager);
	static MediaSourceManager& g_instance_ref = *g_instance;
	return g_instance_ref;
}

void MediaSourceManager::addMedia(const std::string& app, std::shared_ptr<MediaSource> src)
{
	_mediaMap.insert({app, std::move(src)});
}

std::shared_ptr<MediaSource> MediaSourceManager::searchMedia(const std::string& app)
{
	auto it = _mediaMap.find(app);
	if (it != _mediaMap.end())
		return it->second;

	return nullptr;
}

void MediaSourceManager::removeAllMedia()
{
	_mediaMap.clear();
}
