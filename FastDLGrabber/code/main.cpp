#define NOMINMAX
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <iterator>
#include <regex>
#include "Downloader.h"
#include "../ext/cxxopts.hpp"

int main(int argc, char * argv[])
{
	char buf[FILENAME_MAX];
	
#ifdef _WINDOWS_
	GetCurrentDirectoryA(256, buf);
#elif __linux__
#include <unistd.h>
	getcwd(buf, sizeof(buf));
#endif
	const std::string currentDirectory = std::string(buf) + '\\';

	cxxopts::Options options("FastDLGrabber", "Downloads multiple maps from a FastDL");
	options.add_options()
		("h,help",     "This Help page")
		("u,url",     "URL of the FastDL", cxxopts::value<std::string>())
		("o,output",  "Output directory for saved maps", cxxopts::value<std::string>())
		("w,website", "Path of downloaded FastDL index", cxxopts::value<std::string>())
		("p,prefix",  "Map prefix (e.g. surf_ or bhop_)", cxxopts::value<std::string>());
	

	auto result = options.parse(argc, argv);
	std::string fastdlUrl;
	std::string output;
	std::string website;
	std::string prefix;

	if (result["help"].count() == 1)
	{
		std::cout << options.help() << std::endl;
		return 0;
	}
	if (result["url"].count() == 1)
		fastdlUrl = result["url"].as<std::string>();
	if (result["output"].count() == 1)
		output	  = result["output"].as<std::string>();
	if (result["website"].count() == 1)
		website   = result["website"].as<std::string>();
	if (result["prefix"].count() == 1)
		prefix    = result["prefix"].as<std::string>();
	
	CDownloader::DownloadInfo downloadInfo {};
	downloadInfo.fastdlUrl			   = fastdlUrl.empty() ? "http://23.134.192.238/fastdl/maps/" : fastdlUrl;
	downloadInfo.downloadDirectory	   = output.empty()    ? currentDirectory + "maps\\"		  : output;
	downloadInfo.downloadedWebsitePath = website.empty()   ? "fastdl.html"						  : website;
	downloadInfo.prefix				   = prefix.empty()	   ? "surf_"							  : prefix;

	CDownloader *downloader = new CDownloader(downloadInfo);

	downloader->InitCurl();
	downloader->DownloadMaps();

	downloader->Cleanup();
	system("PAUSE");
}