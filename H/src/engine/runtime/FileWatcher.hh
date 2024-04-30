#ifndef FILE_WATCHER_HH
#define FILE_WATCHER_HH

#ifdef ENABLE_AUTOMATIC_FILE_RELOAD

#include <ctime>
#include <functional>
#include <vector>

namespace runtime
{
	/// <summary>
	/// Class to detect when certain files are modified.
	/// </summary>
	class FileWatcher
	{
	public:
		void Update();

		/// <summary>
		/// Declare a function to be called whan a file is modified.
		/// <para/>
		/// The callBackHash parameter allows to have several functions
		/// called for a same file.
		/// Calling again OnFileUpdated with the same file name and
		/// hash will replace the callback instead of adding it.
		/// </summary>
		void OnFileUpdated(const char* fileName, const std::function<void(const char*)>& callBack, unsigned int callBackHash);

	private:
		struct FileData
		{
			const char*							fileName;
			time_t								lastModificationTime;
			std::function<void(const char*)>	callBack;
			unsigned int						callBackHash;
		};

		std::vector<FileData>	m_fileList;
	};
}

#endif // ENABLE_AUTOMATIC_FILE_RELOAD

#endif // FILE_WATCHER_HH
