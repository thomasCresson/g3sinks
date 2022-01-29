/** ==========================================================================
 * This code is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*
 * PUBLIC DOMAIN and Not copywrited.
 * ********************************************* */
#ifndef LOGROTATEFSUTILITY_H
#define LOGROTATEFSUTILITY_H

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace LogRotateFsUtility {
using steady_time_point = std::chrono::time_point<std::chrono::steady_clock>;

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
char* strptime(const char* s, const char* f, struct tm* tm);
#endif

bool fileNameIsValid(std::string_view fileName);
std::string sanitizeFileName(std::string fileName);
std::pair<bool, fs::path> createPathToFile(std::string directoryPath, const std::string& fileName);
std::string formatLogHeader();
std::pair<bool, std::time_t> extractDateFromGzipFileName(const fs::path& filePath, std::string_view logFileName);
void removeExpiredArchives(const fs::path& directoryPath, std::string_view logFileName, uint32_t maxLogArchivesCount);
std::map<long, fs::path> gzipLogFilesInDirectory(const fs::path& directoryPath, std::string_view logFileName);
std::string appendLogExtension(std::string_view logFileNameWithoutExtension);
bool openLogFile(const fs::path& logFilePath, std::ofstream& outstream);
std::unique_ptr<std::ofstream> createAndOpenLogFile(const fs::path& logFilePath);

} // namespace LogRotateFsUtility

#endif // LOGROTATEFSUTILITY_H
