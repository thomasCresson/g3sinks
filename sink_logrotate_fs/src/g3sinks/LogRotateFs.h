/** ==========================================================================
 * This code is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*
 * PUBLIC DOMAIN and Not copywrited.
 * ********************************************* */
#ifndef LOGROTATEFS_H
#define LOGROTATEFS_H

#include <memory>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * The LogRotateFs class is the implementation of a logrotate sink using c++17's std::filesystem library and the ZLIB.
 *
 * This sink is responsible for archiving log files when they reach a certain size and deleting the oldest archives when
 * there are too many of them.
 *
 * This sink allows the user to set the maximum number of log archives and the maximum size of a log file.
 *
 * @note The copy (constructior and assignment) of this sink is implicitly deleted because it is composed of a unique_ptr
 * that is, itself, not copyable.
 */
class LogRotateFs {
public:
	LogRotateFs(const std::string& logFileNameWithoutExtension, const fs::path& logDirectoryPath);
	~LogRotateFs();

	void save(std::string logEntry);
	bool changeLogFile(const fs::path& directory = {}, const std::string& fileName = {});
	fs::path logFilePath();

	void setMaxArchiveLogCount(int max_size);
	int maxArchiveLogCount();

	void setFlushPolicy(size_t flush_policy);
	void flush();

	void setMaxLogSize(int maxLogFileSizeInBytes);
	int maxLogSize();

	bool rotateLog();

private:
	// Forward decl of pimpl.
	struct impl;

	std::unique_ptr<impl> m_impl; ///< Pointer to implementation of the internals of the LogRotateFs class.
};

#endif // LOGROTATEFS_H
