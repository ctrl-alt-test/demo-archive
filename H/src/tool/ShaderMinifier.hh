#ifndef MINIFIER_HH
#define MINIFIER_HH

#if DEBUG

#include "engine/container/Array.hh"
#include <cstdio>
#include <string>
#include <vector>

namespace Tool
{
	/// <summary>
	/// Shader minification.
	/// </summary>
	class Minifier
	{
	public:
		Minifier();
		~Minifier();

		int AddFile(const std::string& filename);
		void MinifyAllFiles() const;

	private:
		void FastMinify(FILE* fp, const char* filename) const;
		void ExternalMinify() const;

		std::vector<std::string> files;
	};
}

#endif // DEBUG

#endif // MINIFIER_HH
