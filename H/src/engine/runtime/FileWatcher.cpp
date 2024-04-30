#ifdef ENABLE_AUTOMATIC_FILE_RELOAD

#include "FileWatcher.hh"
#include "engine/core/Debug.hh"
#include <cstring>
#include <sys/stat.h>

using namespace runtime;

static
bool getFileModificationTime(const char* fileName, time_t* modificationTime)
{
    struct stat attr;
    int result = stat(fileName, &attr);
	*modificationTime = attr.st_mtime;
	return (result == 0);
}

void FileWatcher::Update()
{
	for (unsigned int i = 0; i < m_fileList.size(); i++)
	{
		FileData& fileHandler = m_fileList[i];

		time_t modificationTime;
		if (getFileModificationTime(fileHandler.fileName, &modificationTime))
		{
			if (modificationTime > fileHandler.lastModificationTime)
			{
				LOG_INFO("File has changed: %s.", fileHandler.fileName);
				fileHandler.lastModificationTime = modificationTime;
				fileHandler.callBack(fileHandler.fileName);
			}
		}
		else
		{
			LOG_ERROR("Cannot open file: '%s'.", fileHandler.fileName);
		}
	}
}

void FileWatcher::OnFileUpdated(const char* fileName, const std::function<void(const char*)>& callBack, unsigned int callBackHash)
{
	time_t modificationTime;
	if (!getFileModificationTime(fileName, &modificationTime))
	{
		LOG_FATAL("Cannot monitor file: '%s' cannot be accessed.", fileName);
		return;
	}

	// Avoid duplicates.
	for (unsigned int i = 0; i < m_fileList.size(); i++)
	{
		if (callBackHash == m_fileList[i].callBackHash)
		{
			return;
		}
	}

	LOG_DEBUG("Monitoring %s for caller %u.", fileName, callBackHash);
	FileData fileHandler = { strdup(fileName), modificationTime, callBack, callBackHash };
	m_fileList.push_back(fileHandler);
}

#endif // ENABLE_AUTOMATIC_FILE_RELOAD
