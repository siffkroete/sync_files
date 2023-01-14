/**
*	Created by pei 13.10.2016
*
**/

#ifndef _WPFILESEARCHH_
#define _WPFILESEARCHH_

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <map>
#include <unordered_map>


// Boost - includes 
#include <boost/filesystem.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/seek.hpp>
#include <boost/algorithm/string.hpp>

#include <cctype>

#include "MyFileSystemUtil.h"
#include "MyStringUtil.h"


using namespace myfilesystem;
using namespace mystring;
using namespace std;

namespace wp
{
	struct Stat
	{
		Stat()
			:m_fileCount(0),
			m_fileCount1(0),
			m_fileCount2(0),
			m_rows1Compared(0),
			m_rows2Compared(0),
			m_compared(0),
			m_notExist(0),
			m_diffSize(0),
			m_files_copied(0)
		{
		}

		unsigned int m_fileCount;
		unsigned int m_fileCount1;
		unsigned int m_fileCount2;
		unsigned int m_rows1Compared;
		unsigned int m_rows2Compared;
		unsigned int m_compared;
		unsigned int m_notExist;
		unsigned int m_diffSize;
		unsigned int m_files_copied;

		void clear(void)
		{
			m_fileCount = 0;
			m_fileCount1 = 0;
			m_fileCount2 = 0;
			m_rows1Compared = 0;
			m_rows2Compared = 0;
			m_compared = 0;
			m_notExist = 0;
			m_diffSize = 0;
			m_files_copied = 0;
		}
	};

	struct OutStrings
	{
		OutStrings() : m_diffSizeOutStr("diff size"), m_notExistOutStr("nicht vorhanden")
		{

		};

		string m_diffSizeOutStr;
		string m_notExistOutStr;
	};


	class WpFileSearch
	{
	public:
		WpFileSearch() : m_compareFileSize(true)
		{
		}

		~WpFileSearch()
		{
		}

		const static char s_separator = ';';

		bool m_compareFileSize;
		OutStrings m_outStrings;



		
		void listAllFiles(const string& rootDir, const string& dest, Stat& stat, const bool caseSensitive = true)
		{
			cout << endl;
			const int fileCountPrintRate(10000);
			ofstream ofs(dest);

			const string rd(rootDir);

			boost::filesystem::recursive_directory_iterator itr(rd);
			boost::filesystem::recursive_directory_iterator end_itr;

			stat.clear();

			while (itr != end_itr)
			{
				if (boost::filesystem::is_directory(*itr))
				{
					if (boost::filesystem::is_symlink(*itr))
					{
						itr.no_push();
					}
				}

				string outString = itr->path().string() + "\n";
				ofs << outString;

				try
				{
					++itr;
				}
				catch (bfs::filesystem_error& e)
				{
					std::cout << e.what() << endl;
					itr.no_push();
					try
					{
						++itr;
					}
					catch (...)
					{
						cout << endl << "Schon wieder Fehler!" << endl;
						throw;
					}
				}

				++stat.m_fileCount;
				if ((stat.m_fileCount % fileCountPrintRate) == 0)
				{
					cout << "\r" << stat.m_fileCount << " Dateien gefunden";
				}
			}
			cout << "\r" << "                                                          ";
		}

		void listAllFileSize(const string& rootDir, const string& dest, Stat& stat, const bool caseSensitive)
		{
			cout << endl;
			const int fileCountPrintRate(10000);
			ofstream ofs(dest);
			char buf[31];
			uintmax_t fileSize;
			string strFileSize;
			string outString;

			const string rd = rootDir;

			boost::filesystem::recursive_directory_iterator itr(rd);
			boost::filesystem::recursive_directory_iterator end_itr;
			stat.clear();
			while (itr != end_itr)
			{
				if (boost::filesystem::is_directory(*itr)) // Wenn Ordner
				{
					if (boost::filesystem::is_symlink(*itr))
					{
						itr.no_push();
					}
					outString = itr->path().string() + "\n";
				}
				else  // Wenn Datei
				{
					fileSize = bfs::file_size(itr->path());
					strFileSize = _itoa_s(fileSize, buf, 10);
					outString = itr->path().string() + s_separator + strFileSize + "\n";
				}

				ofs << outString;

				try
				{
					++itr;
				}
				catch (bfs::filesystem_error& e)
				{
					cout << e.what() << endl;
					itr.no_push();
					try
					{
						++itr;
					}
					catch (...)
					{
						cout << endl << "Schon wieder Fehler!" << endl;
						throw;
					}
				}

				++stat.m_fileCount;
				if ((stat.m_fileCount % fileCountPrintRate) == 0)
				{
					cout << "\r" << stat.m_fileCount << " Dateien gefunden";
				}
			}
			cout << "\r" << "                                                          ";
		}

		void compareFiles(const string& textFile1, const string& textFile2, const string& parent1, const string& parent2,
			string& dest, Stat& stat, const bool relative, const bool caseSensitive)
		{
			float fCompared(0.0);
			const int comparePrintRate(1000000);
			try
			{
				ifstream f1(textFile1, ios::in);
				ifstream f2(textFile2, ios::in);
				ofstream r(dest, ios::out | ios::app);

				string line1, line2;
				getline(f1, line1);
				getline(f2, line2);
				f1.clear(); // Brauchts falls Datei schon am Ende (hier eher unwahrscheinlich es den sie hätte nur 1 Eintrag
				f1.seekg(0, ios::beg); // Zurück an den Anfang
				f2.clear(); // Brauchts falls Datei schon am Ende (hier eher unwahrscheinlich es den sie hätte nur 1 Eintrag
				f2.seekg(0, ios::beg); // Zurück an den Anfang

				unordered_set<string> usFile2;

				stat.clear();

				while (getline(f2, line2))
				{
					if (relative)
						line2 = bfs::relative(line2, parent2).string();

					if (!caseSensitive)
						boost::algorithm::to_lower(line2);

					usFile2.insert(line2);
					++stat.m_fileCount2;
				}

				while (getline(f1, line1))
				{
					if (relative)
						line1 = bfs::relative(line1, parent1).string();

					if (!caseSensitive)	
						boost::algorithm::to_lower(line1);


					if (usFile2.find(line1) == usFile2.end()) // Wenn Datei in file2 nicht existiert
					{
						r << m_outStrings.m_notExistOutStr << ": " << line1 << endl;
						++stat.m_notExist;
					}

					++stat.m_fileCount1;
					++stat.m_compared;

					if ((stat.m_compared % comparePrintRate) == 0)
					{
						fCompared = (float)stat.m_compared / 1000.0f;
						cout << "\r" << fCompared << " Tausend Dateien verglichen | ";
						cout << stat.m_notExist << " Datein existieren nicht in" << textFile2;
					}
				}
			}
			catch (...)
			{
				throw;
			}
			cout << "\r" << "                                                          ";
		}


		void compareFileSize(const string& textFile1, const string& textFile2, const string parent1, const string parent2,
			string& dest, Stat& stat, const bool relative, const bool caseSensitive)
		{
			float fCompared(0.0);
			const int comparePrintRate(100000);
			try
			{
				ifstream f1(textFile1, ios::in);
				ifstream f2(textFile2, ios::in);
				ofstream r(dest, ios::out | ios::app);

				string line1, line2, path1, path2;
				getline(f1, line1);
				getline(f2, line2);
				f1.clear(); // Brauchts falls Datei schon am Ende (hier eher unwahrscheinlich es den sie hätte nur 1 Eintrag
				f1.seekg(0, ios::beg); // Zurück an den Anfang
				f2.clear(); // Brauchts falls Datei schon am Ende (hier eher unwahrscheinlich es den sie hätte nur 1 Eintrag
				f2.seekg(0, ios::beg); // Zurück an den Anfang

				unsigned int fileSize1, fileSize2;

				unordered_map<string, unsigned int> umFile2;
				unordered_map<string, unsigned int>::const_iterator itrFile2;

				stat.clear();

				while (getline(f2, line2))
				{
					MyStringUtil::split(line2, path2, fileSize2, s_separator);

					if (relative)
						path2 = bfs::relative(path2, parent2).string();

					if (!caseSensitive)
						boost::algorithm::to_lower(path2);

					umFile2[path2] = fileSize2;
					++stat.m_fileCount2;
				}

				while (getline(f1, line1))
				{
					MyStringUtil::split(line1, path1, fileSize1, s_separator);

					if (relative)
						path1 = bfs::relative(path1, parent1).string();

					if (!caseSensitive)
						boost::algorithm::to_lower(path1);

					itrFile2 = umFile2.find(path1);
					if (itrFile2 == umFile2.end()) // Wenn Pfad in file2 nicht existiert
					{
						r << m_outStrings.m_notExistOutStr << ": " << path1 << endl;
						++stat.m_notExist;
					}
					else if (m_compareFileSize)// Wenn Pfad in file2 existiert 
					{// dann Grösse vergleichen
						fileSize2 = itrFile2->second;
						if (fileSize1 != fileSize2) // Wenn Grösse nicht übereinstimmt
						{
							r << m_outStrings.m_diffSizeOutStr << ": " << line1 << " bytes | in 2: " << fileSize2 << " bytes" << endl;
							++stat.m_diffSize;
						}
					}

					++stat.m_fileCount1;
					++stat.m_compared;

					if ((stat.m_compared % comparePrintRate) == 0)
					{
						fCompared = (float)stat.m_compared / 1000.0f;
						cout << "\r" << fCompared << " Tausend Dateien verglichen | ";
						cout << stat.m_notExist << " Datein existieren nicht in der " << textFile2;
						cout << endl << stat.m_diffSize << " Datein haben verschiedene Grössen" << endl;
					}
				}
			}
			catch (...)
			{
				throw;
			}
			cout << "\r" << "                                                          ";
		}

		void out(const string& outputString, const string& dest)
		{
			try
			{
				fstream f(dest, ios::app);
				f << outputString << "\n";
			}
			catch (...)
			{
				throw;
			}
		}
	};
} // namespace wp

#endif