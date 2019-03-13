#include "Downloader.h"
#define _CRT_SECURE_NO_WARNINGS

CDownloader::CDownloader(DownloadInfo _info)
	: m_dlInfo(_info)
{
	if (!m_dlInfo.downloadDirectory.empty())
	{
		if (!fs::is_directory(m_dlInfo.downloadDirectory) || !fs::exists(m_dlInfo.downloadDirectory))
		{
			fs::create_directory(m_dlInfo.downloadDirectory);
		}
	}
}

CDownloader::~CDownloader()
{
}

CURLcode CDownloader::InitCurl()
{
	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if (result != 0)
	{
		std::cout << "Failed curl_global_init() with return code: " << result << std::endl;
		return result;
	}
	else
	{
		m_pCurl = curl_easy_init();
		if (!m_pCurl)
		{
			std::cout << "Failed curl_easy_init()" << std::endl;
			return result;
		}
#ifdef _DEBUG
		else
		{
			std::cout << "Curl initialized successfully!" << std::endl;
		}
#endif
	}

	return result;
}

CURLcode CDownloader::DownloadMaps()
{
	CURLcode result = DownloadFileFromWebsite(m_dlInfo.fastdlUrl, m_dlInfo.downloadedWebsitePath);
	if (result == CURLcode::CURLE_OK)
	{
		std::cout << "Downloaded FastDL index successfully!" << std::endl;

		for (std::string link : CHelper::ExtractHyperlinks(m_dlInfo.downloadedWebsitePath))
		{
			if (CHelper::StartsWith(link, m_dlInfo.prefix) && (CHelper::EndsWith(link, ".bz2") || CHelper::EndsWith(link, ".bz2")))
			{
				std::string fullLink = m_dlInfo.fastdlUrl + link;
				std::string filePath = m_dlInfo.downloadDirectory + link;
			
				result = DownloadFileFromWebsite(fullLink, filePath);
			}
		}
	}
	return result;
}

CURLcode CDownloader::DownloadFileFromWebsite(std::string _websiteUrl, std::string _outputPath)
{
	CURLcode result = CURLcode::CURLE_OK;
	if (m_pCurl)
	{
		struct stat buffer;
		if (stat(_outputPath.c_str(), &buffer) != 0)
		{
			fopen_s(&m_pFile, _outputPath.c_str(), "wb");
			curl_easy_setopt(m_pCurl, CURLOPT_URL, _websiteUrl.c_str());
			curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, &CDownloader::WriteData);
			curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, m_pFile);
			result = curl_easy_perform(m_pCurl);
			fclose(m_pFile);

			if (!CHelper::EndsWith(_outputPath, ".html") && result == CURLcode::CURLE_OK)
			{
				std::cout << "Map: " << _outputPath << " downloaded!" << '\n';
			}
			else
			{
				return result;
			}
		}
		else if(!CHelper::EndsWith(_outputPath, ".html") && stat(_outputPath.c_str(), &buffer) == 0)
		{
			std::cout << "Map: " << _outputPath << " already exists! Skipping..." << std::endl;
		}
	}

#ifdef _DEBUG
	if (result != 0)
	{
		std::cout << "Curl Opt failed with error code: " << result << std::endl;
	}
#endif // _DEBUG

	return result;
}

CURLcode CDownloader::Cleanup()
{
	remove(m_dlInfo.downloadedWebsitePath.c_str());
	curl_easy_cleanup(m_pCurl);
	m_pCurl = nullptr;
	m_pFile = nullptr;
	return CURLcode::CURLE_OK;
}

size_t CDownloader::WriteData(void * _ptr, const size_t _size, const size_t _nmemb, FILE * _stream)
{
	size_t written = fwrite(_ptr, _size, _nmemb, _stream);
	return written;
}
