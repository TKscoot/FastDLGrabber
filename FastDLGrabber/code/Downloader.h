#pragma once
#include <vector>
#include <assert.h>
#include <curl/curl.h>
#include <experimental/filesystem>
#include "Helper.h"

namespace fs = std::experimental::filesystem;

class CDownloader
{

public:
	struct DownloadInfo
	{
		std::string fastdlUrl;
		std::string downloadDirectory;
		std::string downloadedWebsitePath;
		std::string prefix;
	};
	CDownloader(DownloadInfo _info);
	~CDownloader();

	CURLcode InitCurl();
	CURLcode DownloadMaps();
	CURLcode Cleanup();


private:
	CURLcode DownloadFileFromWebsite(std::string _websiteUrl, std::string _outputPath);
	static size_t WriteData(void *_ptr, const size_t _size, const size_t _nmemb, FILE *_stream);

	DownloadInfo m_dlInfo;
	FILE *m_pFile = nullptr;
	CURL *m_pCurl = nullptr;
};

