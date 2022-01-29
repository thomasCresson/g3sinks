/** ==========================================================================
 * This code is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*
 * PUBLIC DOMAIN and Not copywrited.
 * ********************************************* */
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <zlib.h>

#include "g3sinks/LogRotateFs.h"
#include "g3log/time.hpp"
#include "g3sinks/LogRotateFsUtility.h"

using namespace LogRotateFsUtility;

// LogRotateFs::impl struct DEFINITION
/**
 * @brief The Real McCoy Background worker, while g3::LogWorker gives the
 * asynchronous API to put job in the background the LogRotateFs::impl
 * does the actual background thread work.
 *
 * @note Since LogRotateFs::impl is stored in a unique_ptr inside LogrotateFs,
 * there is no need to prevent its copy (constructor and assignment). Since it's
 * a private inner struct, LogRotateFs has complete control over it.
 */
struct LogRotateFs::impl {
	impl(const std::string& logFileNameWithoutExtension, const fs::path& logDirectoryPath, size_t flushPolicy = 1);
	~impl();

	void setMaxArchiveLogCount(int maxSize);
	int getMaxArchiveLogCount();
	void setMaxLogSize(int maxLogSizeInBytes);
	int getMaxLogSize();

	void fileWrite(std::string message);
	void fileWriteWithoutRotate(std::string message);
	void flushPolicy();
	void setFlushPolicy(size_t flushPolicy);
	void flush();

	bool changeLogFile(const fs::path& directory, const std::string& fileName);
	fs::path logFilePath();
	bool archiveLog();

	void addLogFileHeader();
	bool rotateLog();
	void setLogSizeCounter();
	bool createCompressedFile(const std::filesystem::__cxx11::path& gzipFileName);
	std::ofstream& filestream() { return *(m_ofStreamUPtr.get()); }

	fs::path m_logFilePath; ///< Path to the current log file (including its name).
	std::unique_ptr<std::ofstream> m_ofStreamUPtr; ///< Pointer to the file stream writer.
	int m_maxLogSizeInBytes; ///< Maximum size of a log file in bytes before archiving it.
	int m_maxArchiveLogCount; ///< Maximum number of log file archives to keep.
	std::streamoff m_curLogSize; ///< Size of the current log file.
	size_t m_flushPolicy; ///< Defines the flush policy for the file stream writer.
	                      ///< Flushing of the file stream writer will occur according to flush policy:
	                      ///<     - 0 is never (system decides, and when there is a log rotation)
	                      ///<     - 1 .. N means every x entry (1 is every write, 2 is every two writes etc)
	                      ///< Default behavior is to flush after every entry.
	size_t m_flushPolicyCounter; ///< Counting the number of writes before flushing (according to the flush policy).
};

// LogRotateFs INTERFACE
/**
 * @brief Constructs a LogRotateFs placing the log files \p logFileNameWithoutExtension in the directory \p logDirectoryPath.
 * @param logFileNameWithoutExtension Log file name without extension.
 * @param logDirectoryPath Directory path where the log files are placed.
 */
LogRotateFs::LogRotateFs(const std::string& logFileNameWithoutExtension, const fs::path& logDirectoryPath)
    : m_impl(std::make_unique<impl>(logFileNameWithoutExtension, logDirectoryPath))
{
}

LogRotateFs::~LogRotateFs() { std::cerr << "\nExiting, log location: " << m_impl->m_logFilePath << std::endl; }

/**
 * @brief Saves the \p logEntry in the current log file.
 * @param logEntry Content to write to the file.
 */
void LogRotateFs::save(std::string logEntry) { m_impl->fileWrite(logEntry); }

/**
 * @brief Attempts to create a new log file with \p fileName into \p directory.
 * @param directory Directory in which the new log file is created.
 * @param fileName Name of the new log file.
 * @return True if the new log file has been correctly created, false otherwise.
 */
bool LogRotateFs::changeLogFile(const fs::path& directory, const std::string& fileName) { return m_impl->changeLogFile(directory, fileName); }

/**
 * @return The current file name the sink writes to.
 */
fs::path LogRotateFs::logFilePath() { return m_impl->logFilePath(); }

/**
 * @brief Set the max number of archived logs to \p max_count.
 * @param max_count Maximum number of archived logs.
 * @note
 */
void LogRotateFs::setMaxArchiveLogCount(int max_count) { m_impl->setMaxArchiveLogCount(max_count); }

/**
 * @return The maximum number of archived logs.
 */
int LogRotateFs::maxArchiveLogCount() { return m_impl->getMaxArchiveLogCount(); }

/**
 * @brief Flush policy: Default is every single time (i.e. policy of 1).
 *
 * If the system logs A LOT then it is likely better to allow for the system to buffer and write
 * all the entries at once.
 *
 * 0: System decides, potentially very long time
 * 1....N: Flush logs every n entry
 */
void LogRotateFs::setFlushPolicy(size_t flush_policy) { m_impl->setFlushPolicy(flush_policy); }

/**
 * @brief Force flush of log entries. This should normally be policed with the
 * LogRotateFs::setFlushPolicy but is great for unit testing and if there are
 * special circumstances where you want to see the logs faster than the flush_policy.
 */
void LogRotateFs::flush() { m_impl->flush(); }

/**
 * @brief Set the maximum size of a log file in bytes to \p max_file_size.
 * @param max_file_size Maximum size in bytes of a log file.
 */
void LogRotateFs::setMaxLogSize(int maxLogFileSizeInBytes) { m_impl->setMaxLogSize(maxLogFileSizeInBytes); }

/**
 * @return The maximum size of a log file in bytes.
 */
int LogRotateFs::maxLogSize() { return m_impl->getMaxLogSize(); }

/**
 * @brief Archives the current log file, create a new one and delete all the expired archives.
 * @return True on success, false on failure.
 */
bool LogRotateFs::rotateLog() { return m_impl->rotateLog(); }

// LogRotateFs IMPLEMENTATION
/**
 * @brief Constructs the private implementation pointer to a LogRotateFs. This constructor initializes the log file named after \p logFileNameWithoutExtension
 * and located in \p logDirectoryPath. It also sets the \p flushPolicy of the internal stream.
 * @param logFileNameWithoutExtension Log file name stripped from all its special characters and without extension.
 * @param logDirectoryPath Path to the directory where the log file is created.
 * @param flushPolicy Indicates the number of writes to be done before flushing the internal stream (0 means that the system will flush the stream when it sees
 * fit).
 */
LogRotateFs::impl::impl(const std::string& /*logFileNameWithoutExtension*/, const fs::path& /*logDirectoryPath*/, size_t /*flushPolicy*/)
    : m_logFilePath()
    , m_ofStreamUPtr()
    , m_maxLogSizeInBytes()
    , m_maxArchiveLogCount()
    , m_curLogSize()
    , m_flushPolicy()
    , m_flushPolicyCounter()
{
	// TODO: Implement
}

/**
 * @brief Set the maximum number of archived logs to keep. The older files are to be erased.
 * @param maxNbOfArchives Maximum number of archived logs to keep.
 */
void LogRotateFs::impl::setMaxArchiveLogCount(int maxNbOfArchives) { m_maxArchiveLogCount = maxNbOfArchives; }

/**
 * @return Retrieves the maximum number of archived logs to keep.
 */
int LogRotateFs::impl::getMaxArchiveLogCount() { return m_maxArchiveLogCount; }

/**
 * @brief Set the maximum size of a log file in bytes. When this size is reached, the log file will be rotated.
 * @param maxLogSizeInBytes Maximum size of a log file in bytes.
 */
void LogRotateFs::impl::setMaxLogSize(int maxLogSizeInBytes) { m_maxLogSizeInBytes = maxLogSizeInBytes; }

/**
 * @return Retrieves the maximum size of a log file in bytes.
 */
int LogRotateFs::impl::getMaxLogSize() { return m_maxLogSizeInBytes; }

LogRotateFs::impl::~impl()
{
	// TODO: Implement
}

/**
 * @brief Writes \p message in the current log file. The file is rotated before writing in the current log file.
 * @param message Message to write to the log file.
 */
void LogRotateFs::impl::fileWrite(std::string /*message*/)
{
	// TODO: Implement
}

/**
 * @brief Writes \p message in the current log file without rotating it.
 * @param message Message to write to the log file.
 */
void LogRotateFs::impl::fileWriteWithoutRotate(std::string /*message*/)
{
	// TODO: Implement
}

/**
 * @brief Flushes the inner stream according to the flush policy.
 */
void LogRotateFs::impl::flushPolicy()
{
	// TODO: Implement
}

/**
 * @brief Flushes the inner stream and set the new \p flushPolicy.
 * @param flushPolicy Number of writes before flushing the stream (0 let the system flush the stream whenever it sees fit).
 */
void LogRotateFs::impl::setFlushPolicy(size_t /*flushPolicy*/)
{
	// TODO: Implement
}

/**
 * @brief Forcefully flushes the inner stream.
 */
void LogRotateFs::impl::flush()
{
	// TODO: Implement
}

/**
 * @brief Attempts to create a new log file with \p fileName into \p directory.
 * @param directory Directory in which the new log file is created.
 * @param fileName Name of the new log file.
 * @return True if the new log file has been correctly created, false otherwise.
 */
bool LogRotateFs::impl::changeLogFile(const fs::path& /*directory*/, const std::string& /*fileName*/)
{
	// TODO: Implement
	return false;
}

/**
 * @brief Rotates the logs once they have exceeded the maximum size.
 * @return True on success, false otherwise.
 */
bool LogRotateFs::impl::rotateLog()
{
	// TODO: Implement
	return false;
}

/**
 * @brief Updates the internal counter for the g3 log size.
 */
void LogRotateFs::impl::setLogSizeCounter()
{
	// TODO: Implement
}

/**
 * @brief Archives the current log file in the gzip format into the file \p gzipFilePath.
 * @param gzipFilePath Complete path to the gzip file to be created.
 * @return True in case of success, false otherwise.
 */
bool LogRotateFs::impl::createCompressedFile(const fs::path& /*gzipFilePath*/)
{
	// TODO: Implement
	return false;
}

/**
 * @return The complete path to the current log file.
 */
fs::path LogRotateFs::impl::logFilePath() { return m_logFilePath; }

/**
 * @brief Inserts the header to the beginning of the log file (should be called first whenever a new log file is created).
 */
void LogRotateFs::impl::addLogFileHeader()
{
	// TODO: Implement
}
