/** ==========================================================================
 * This code is PUBLIC DOMAIN to use at your own risk and comes
 * with no warranties. This code is yours to share, use and modify with no
 * strings attached and no restrictions or obligations.
 * ============================================================================*
 * PUBLIC DOMAIN and Not copywrited.
 * ********************************************* */
#include <fstream>

#include "g3sinks/LogRotateFsUtility.h"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__)) && !defined(__MINGW32__)
// http://stackoverflow.com/questions/321849/strptime-equivalent-on-windows
char* strptime(const char* s, const char* f, struct tm* tm)
{
	// Isn't the C++ standard lib nice? std::get_time is defined such that its
	// format parameters are the exact same as strptime. Of course, we have to
	// create a string stream first, and imbue it with the current C locale, and
	// we also have to make sure we return the right things if it fails, or
	// if it succeeds, but this is still far simpler an implementation than any
	// of the versions in any of the C standard libraries.
	std::istringstream input(s);
	input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
	input >> std::get_time(tm, f);

	if (input.fail()) {
		return nullptr;
	}

	return (char*)(s + input.tellg());
}
#endif

/**
 * @brief Checks for the validity of the \p fileName.
 * @param fileName File name to check.
 * @return True if the file name doesn't contain any illegal character or is empty, false otherwise.
 */
bool LogRotateFsUtility::fileNameIsValid(std::string_view /*fileName*/)
{
	// TODO: implement
	return false;
}

/**
 * @brief Sanitizes the \p fileName by stripping it from any space character or any path separator ("/" or "\").
 * @param fileName File name to sanitize.
 * @return The sanatized file name if valid, an empty string otherwise.
 */
std::string LogRotateFsUtility::sanitizeFileName(std::string /*fileName*/)
{
	// TODO: implement
	return {};
}

/**
 * @brief Creates a path to a file given the \p directoryPath and the \p fileName. The path separators in \p directoryPath are unified to "/".
 * @param directoryPath Full path to the directory containing the file.
 * @param fileName Name of the file in the directory path.
 * @return A pair containing a boolean (for success) and the full canonical path to \p directoryPath / \p fileName.
 */
std::pair<bool, fs::path> LogRotateFsUtility::createPathToFile(std::string /*directoryPath*/, const std::string& /*fileName*/)
{
	// TODO: implement
	return {};
}

/**
 * @brief Formats the first line of a log file.
 * @return The first line to be inserted in the log file.
 */
std::string LogRotateFsUtility::formatLogHeader()
{
	// TODO: implement
	return {};
}

/**
 * @brief Extracts the date contained in the compressed file in \p gzipFilePath if it contains \p logFileName in its name.
 * @param gzipFilePath Full path to the file.
 * @param logFileName Name of the log file.
 * @return A pair containing a boolean (representing the success of the extraction) and the date in a std::time_t.
 */
std::pair<bool, std::time_t> LogRotateFsUtility::extractDateFromGzipFileName(const fs::path& /*gzipFilePath*/, std::string_view /*logFileName*/)
{
	// TODO: implement
	return {};
}

/**
 * @brief Removes the oldest archives from \p directoryPath containing \p logFileName when there is more than \p maxLogArchivesCount archives in the
 * directory.
 * @param directoryPath Path to the directory containing the archives.
 * @param logFileName Log file name contained in the archives names.
 * @param maxLogArchivesCount Maximum number of archives that are allowed.
 */
void LogRotateFsUtility::removeExpiredArchives(const fs::path& /*directoryPath*/, std::string_view /*logFileName*/, uint32_t /*maxLogArchivesCount*/)
{
	// TODO: implement
}

/**
 * @brief Retrieves the list of all the gzip files in \p directoryPath which contains \p logFileName in it.
 * @param directoryPath Directory containing the gzip files.
 * @param logFileName Name of the log file that should be contained in the gzip files name.
 * @return A map containing the paths of the gzip files contained in \p directoryPath and sorted by dates in ascending order.
 */
std::map<std::time_t, fs::path> LogRotateFsUtility::gzipLogFilesInDirectory(const fs::path& /*directoryPath*/, std::string_view /*logFileName*/)
{
	// TODO: implement
	return {};
}

/**
 * @brief Appends the ".log" extension to \p logFileNameWithoutExtension and returns it.
 * @param logFileNameWithoutExtension Log file name without any extension.
 * @return The log file name with the ".log" file extension.
 */
std::string LogRotateFsUtility::appendLogExtension(std::string_view /*logFileNameWithoutExtension*/)
{
	// TODO: implement
	return {};
}

/**
 * @brief Opens, in writing mode, the log file \p logFilePath linked to \p outstream.
 * @param logFilePath Full path to the log file.
 * @param outstream Stream linked to the log file.
 * @return True if the log file has been correctly opened, false otherwise.
 */
bool LogRotateFsUtility::openLogFile(const fs::path& /*logFilePath*/, std::ofstream& /*outstream*/)
{
	// TODO: implement
	return false;
}

/**
 * @brief Creates and opens the log file, in writing mode, from \p logFilePath.
 * @param logFilePath Full path to the log file to create and open.
 * @return The stream linked to the log file that has been created and opened.
 */
std::unique_ptr<std::ofstream> LogRotateFsUtility::createAndOpenLogFile(const fs::path& /*logFilePath*/)
{
	// TODO: implement
	return {};
}
