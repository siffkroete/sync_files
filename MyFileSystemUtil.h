#ifndef _MYFILESYSTEMH_
#define _MYFILESYSTEMH_

#include <iostream>

// Boost includes
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "MyStringUtil.h"

using namespace std;

namespace myfilesystem
{
	namespace bfs = boost::filesystem;

	class MyFileSystemUtil
	{
	public:

		MyFileSystemUtil()
		{
		}

		~MyFileSystemUtil()
		{
		}
		
		static bool exists(const string& path)
		{
			bfs::path p(path);
			if (!p.empty() && bfs::exists(path)) { return true; }
			else { return false; }
		}

		static bool isEmpty(const string& path)
		{
			if (bfs::path(path).empty()) { return true; }
			else { return false; }
		}

		static string getPreferredSlash(void)
		{
			return bfs::path("/").make_preferred().string();
		}

		static bool create(const string& path)
		{
			bool r(false);
			try
			{
				auto p = bfs::path(path);
				if(!p.empty() && !bfs::exists(path)) // Wennn Pfas-String nicht leer und wenn Datei noch nicht existiert
				{
					if (p.has_filename()) // Wenn Datei
					{
						auto parent = p.parent_path();
						r = bfs::create_directories(parent);
						std::ofstream outfile(path);
						if (!outfile)
							throw(std::exception("Die Datei konnte nicht erstellt werden! Funktion: MyFileSystemUtil::clearFile()"));
						outfile.close();
					}
					else if(bfs::is_directory(path)) // Wenn Ordner
					{
						r = bfs::create_directories(p);
					}
					else
					{
						throw(std::exception("Der Pfad hat weder eine Dateinamen noch ist es ein Ordner! \
						Funktion: MyFileSystem::create()"));
					}
				}
			}
			catch (std::exception& e)
			{
				printf("\Fehler in MyFileSystemUtil::create()");
				throw e;
			}
			return r;
		}

		// Useful utility for creating directories for storing the output files
		static bool create_directory_from_file(const string& path)
		{
			auto p = bfs::path(bfs::path(path).parent_path());
			if (!p.empty() && !bfs::exists(p))
			{
				if(!bfs::create_directories(p))
					throw(std::exception("Der Ordner konnte nicht erstellt werden! \
					 Funktion: MyFileSystemUtil::create_directory_from_file()"));
				return true;
			}
			return false;
		}

		static bool clearFile(const string& path)
		{
			try
			{
				auto p = bfs::path(path);
				if (!p.empty() && bfs::exists(path) && !bfs::is_empty(path) && !bfs::is_directory(path))
				{
					removeFile(path);
					std::ofstream outfile(path);
					if (!outfile)
						throw(std::exception("Die Datei konnte nicht ordentlich gel\x94scht werden! Funktion: SysUtil::clearFile()"));
					outfile.close();
					return true;
				}
				return false;
			}
			catch (std::exception& e)
			{
				printf("\nError in SysUtil::clearFile()\n");
				throw e;
			}
		}


		// Remove file in a independent manner
		static int removeFile(const string& path)
		{
			return removeDir(path);
		}

		// Clear Directory in a Os-independent manner
		static unsigned int clearDir(const string& path)
		{
			int count = 0;
			bfs::path path_to_remove(path);
			if (!isEmpty(path) && exists(path))
			{
				for (bfs::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it)
				{
					bfs::remove_all(it->path());
					++count;
				}
			}
			return count;
		}

		// Remove Directory in a Os-independent manner
		static int removeDir(const string& path)
		{
			bfs::path path_to_remove(path);
			if (!isEmpty(path) && exists(path))
			{
				int fileCount = boost::filesystem::remove_all(path);
				return fileCount;
			}
			return 0;
		}



		// Yields all paths in directory in a Os-independent manner
		static int getAllFilePaths(const string& sourceDir, std::vector<string>& fileNames)
		{
			if (!isEmpty(sourceDir) && exists(sourceDir) && bfs::is_directory(sourceDir))
			{
				bfs::path source(sourceDir);
				for (bfs::directory_iterator end_dir_it, it(source); it != end_dir_it; ++it) {
					fileNames.push_back(it->path().string());
				}
				return fileNames.size();
			}
			return 0;
		}

		// Gibt alle Ordner zurück aus sourceDir
		static int getAllFileNames(const string& sourceDir, std::vector<string>& fileNames)
		{
			// namespace bfs = boost::filesystem;
			// bfs::path p(sourceDir);
			// std::cout << "filename and extension : " << p.filename() << std::endl; // file.ext
			// std::cout << "filename only          : " << p.stem() << std::endl;     // file

			if (!isEmpty(sourceDir) && exists(sourceDir) && bfs::is_directory(sourceDir))
			{
				bfs::path source(sourceDir);
				for (bfs::directory_iterator end_dir_it, it(source); it != end_dir_it; ++it)
				{
					fileNames.push_back(it->path().filename().string());
				}
				return fileNames.size();
			}
			return 0;
		}

		// Gibt alle Ordner zurück aus sourceDir
		static int getAllDirNames(const string& sourceDir, std::vector<string>& fileNames)
		{
			// namespace bfs = boost::filesystem;
			// bfs::path p(sourceDir);
			// std::cout << "filename and extension : " << p.filename() << std::endl; // file.ext
			// std::cout << "filename only          : " << p.stem() << std::endl;     // file

			if (!isEmpty(sourceDir) && exists(sourceDir) && bfs::is_directory(sourceDir))
			{
				bfs::path source(sourceDir);
				for (bfs::directory_iterator end_dir_it, it(source); it != end_dir_it; ++it)
				{
					if (bfs::is_directory(it->path()))
					{
						fileNames.push_back(it->path().filename().string());
					}
				}
				return fileNames.size();
			}
			return 0;
		}

		static string getFileName(const string& path)
		{
			
			if (!isEmpty(path)) // Wenn Pfad nicth leer
			{ 
				return(bfs::path(path).filename().string()); // Den Namen der Datei zurückgeben
			}
			else // Wenn Pfad leer
			{
				return "";
			}
		}

		static string getExtension(const string path)
		{
			string _path(path);
			mystring::MyStringUtil::trimWhiteSpaceRight(_path);
			bfs::path p(_path);
			string ext = p.extension().string();
			return ext;
		}
		
		static void normalizePath(string& path,const bool caseSensitive)
		{
			auto p = bfs::path(path);
			p.make_preferred();
			path = p.string();
		}

		static bool is_directory(const string path)
		{
			return boost::filesystem::is_directory(path);
		}

		static bool copy_file(const string source_path, const string dest_path)
		{
			const auto copyOptions = bfs::copy_options::update_existing | bfs::copy_options::recursive;  // | fs::copy_options::directories_only
			return bfs::copy_file(source_path, dest_path, copyOptions); // Von Rechts nach Links kopieren
		}

		static void copy_directory(const bfs::path& sourceDir, const bfs::path& destinationDir)
		{
			if (!bfs::exists(sourceDir) || !bfs::is_directory(sourceDir))
			{
				throw std::runtime_error("Source directory " + sourceDir.string() + " does not exist or is not a directory");
			}
			if (bfs::exists(destinationDir))
			{
				throw std::runtime_error("Destination directory " + destinationDir.string() + " already exists");
			}
			if (!bfs::create_directory(destinationDir))
			{
				throw std::runtime_error("Cannot create destination directory " + destinationDir.string());
			}

			for (const auto& dirEnt : bfs::recursive_directory_iterator{ sourceDir })
			{
				const auto& path = dirEnt.path();
				auto relativePathStr = path.string();
				boost::replace_first(relativePathStr, sourceDir.string(), "");
				bfs::copy(path, destinationDir / relativePathStr);
				// bfs::copy(path, destinationDir + "/" + relativePathStr);
			}
		}
		
		
	};
}

#endif
