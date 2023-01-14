/**
*	Created by pei 24.10.2016
*
**/


#ifndef _FILESEARCHUTILH_
#define _FILESEARCHUTILH_

#include "WpFileSearch.h"
#include "Timer.h"
#include "Log.h"
#include "MyStringUtil.h"
#include <thread>

// Boost - includes 
#include <boost/filesystem.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/iostreams/seek.hpp>
#include <boost/algorithm/string.hpp>

using namespace mystring;


const std::string fehlt_links_prefix = "nicht vorhanden";
const std::string fehlt_rechts_prefix = "neu";




namespace wp
{
	namespace ba = boost::algorithm;
	namespace fs = boost::filesystem;

	class FileSearchUtil
	{
	public:
		enum {MAX_PATH = 255};
		enum cmd {CMD_MENU = 0, CMD_LIST = 1, CMD_COMPARE, CMD_ALL, CMD_1_AND_2, CMD_SYNC, CMD_ONLY_SYNC };
		enum menu {MENU_LIST = 1, MENU_COMPARE, MENU_SYNC, MENU_ENABLE_DISABLE_RELATIVE, MENU_ENABLE_DISABLE_CASE, MENU_ENABLE_DISABLE_SIZE, END};

		static WpFileSearch s_fs;

		static Timer s_timer;
		static bool s_enabledMenu;
		static bool s_enabledRelative;
		static bool s_caseSensitive;
		static bool s_enabledFileSize;
		static unsigned int s_cycleCount;
		static string s_rootDir[2];
		static string s_fileList[2];
		static string s_standardFileName[2];
		static string s_standardDirName[2];
		static string s_result;
		static string s_ext;
		static Stat s_stat;

		static void init(void)
		{
			s_enabledMenu = (atoi(getValueFromIni("cmd").c_str()) == 0 ? true : false);
			s_enabledRelative = (getValueFromIni("relativePath") == "enabled" || getValueFromIni("relativePath") == "true" ? true : false);
			s_caseSensitive = (getValueFromIni("caseSensitive") == "enabled" || getValueFromIni("caseSensitive") == "true" ? true : false);
			s_enabledFileSize = (getValueFromIni("fileSize") == "enabled" || getValueFromIni("fileSize") == "true"  ? true : false);

			s_standardFileName[0] = "fileList1";
			s_standardFileName[1] = "fileList2";
			s_standardDirName[0] = "dir1";
			s_standardDirName[1] = "dir2";

			s_cycleCount = 0;
		}

		static void listFiles(const string s_rootDir, const string dest, Stat& s_stat,float& time)
		{
			s_timer.start();
			if (s_enabledFileSize == false)
			{
				s_fs.listAllFiles(s_rootDir, dest, s_stat, s_caseSensitive);
			}
			else
			{
				s_fs.listAllFileSize(s_rootDir, dest, s_stat, s_caseSensitive);
			}
			time = s_timer.getTimeInSek();
		}

		static void compareFiles(const string list1, const string list2, const string parent1, const string parent2, string dest, Stat& stat, float& time)
		{
			s_timer.start();
			if (s_enabledFileSize == false)
			{
				s_fs.compareFiles(list1, list2, parent1, parent2, dest, stat, s_enabledRelative, s_caseSensitive);
			}
			else
			{
				s_fs.compareFileSize(list1, list2, parent1, parent2, dest, stat, s_enabledRelative, s_caseSensitive);
			}
			time = s_timer.getTimeInSek();
		}

		static void listFiles(void)
		{
			float timeElapsed(0.0);
			char cRootDir[MAX_PATH], cFileList[MAX_PATH];

			if (s_enabledMenu)
			{
				cout << endl << "root directory: ";
				cin.getline(cRootDir, sizeof(cRootDir));
				s_rootDir[s_cycleCount] = cRootDir;
				checkDir(s_rootDir[s_cycleCount]);

				cout << endl << "Ziel: ";
				cin.getline(cFileList, sizeof(cFileList));
				s_fileList[s_cycleCount] = cFileList;
				checkFile(s_fileList[s_cycleCount], s_standardFileName[s_cycleCount]);
			}
			else
			{
				s_fileList[s_cycleCount] = getValueFromIni(s_standardFileName[s_cycleCount]);
				s_rootDir[s_cycleCount] = getValueFromIni(s_standardDirName[s_cycleCount]);
				checkFile(s_fileList[s_cycleCount], s_standardFileName[s_cycleCount]);
				checkDir(s_rootDir[s_cycleCount]);
			}

			listFiles(s_rootDir[s_cycleCount], s_fileList[s_cycleCount], s_stat, timeElapsed);

			cout << endl << "filecount: " << s_stat.m_fileCount;
			cout << endl << "Zeit: " << timeElapsed;
			cout.precision(2);
			cout << endl << "Tausend files / s: ";
			cout << ((float)s_stat.m_fileCount / 1000.0f) / (timeElapsed);
			cout << endl << "Das Ergebniss ist in " << s_fileList[s_cycleCount] << " aufgelistet" << endl;
		}

		static void compareFiles(void)
		{
			float timeElapsed(0.0);
			char cResult[MAX_PATH], cFileList[MAX_PATH];

			if (s_enabledMenu)
			{
				for (int i = 0; i < 2; ++i)
				{ // Die Namen der Listen abfragen
					cout << endl << "Liste" << s_cycleCount + 1 << " : ";
					cin.getline(cFileList, sizeof(cFileList));
					s_fileList[i] = cFileList;
					checkFile2(s_fileList[i], s_ext);
					cout << endl;
				}

				cout << endl << "Resultat (txt-file): ";
				cin.getline(cResult, sizeof(cResult));
				s_result = cResult;
				checkFile2(s_result, s_ext);
			}
			else
			{
				s_fileList[0] = getValueFromIni("fileList1");
				s_fileList[1] = getValueFromIni("fileList2");
				s_result = getValueFromIni("result");
				checkFile2(s_fileList[0], s_ext);
				checkFile2(s_fileList[1], s_ext);
				checkFile(s_result, s_ext);
			}

			string parent1, parent2;
			getParent(parent1, parent2);

			Stat stat1, stat2;


			compareFiles(s_fileList[0], s_fileList[1], parent1, parent2, s_result, s_stat, timeElapsed);
			stat1 = s_stat; // Statistiken zwischenspeichern
			s_stat.clear(); 
			s_fs.m_compareFileSize = false;
			s_fs.m_outStrings.m_notExistOutStr = fehlt_rechts_prefix;
			compareFiles(s_fileList[1], s_fileList[0], parent2, parent1, s_result, s_stat, timeElapsed);
			s_fs.m_outStrings.m_notExistOutStr = fehlt_rechts_prefix;
			s_fs.m_compareFileSize = true;
			stat2 = s_stat;
			s_stat.clear(); 
			
			cout << endl << "compared 1 with 2: " << stat1.m_compared;
			cout << endl << "compared 2 with 1: " << stat2.m_compared;
			cout << endl << "notExist in 2: " << stat1.m_notExist;
			cout << endl << "notExist in 1: " << stat2.m_notExist;
			if (s_enabledFileSize) cout << endl << "different size: " << stat1.m_diffSize;
			cout << endl << "filecount1: " << stat1.m_fileCount1;
			cout << endl << "filecount2: " << stat1.m_fileCount2;
			cout << endl << "Zeit: " << timeElapsed;
			cout.precision(2);
			cout << endl << "Tausend files compared / s: ";
			cout << ((float)stat1.m_compared + stat2.m_compared/ 1000.0f) / (timeElapsed);
			cout << endl << "Das Resultat finden Sie in " << s_result << endl;
		}


		// (Zum Bildlichen Verständnis können wir den 1 Ordner als "links" bezeichnen und den 2 Ordner als "rechts".)
		// Diese Funktion kopiert alle fehlenden Dateien von Links nach Rechts und umgekehrt.
		static int sync_files() 
		{
			// Hier stehen die fehlenden Dateien, wobei das Präfix "nicht vorhanden:" bedeutet Rechts nicht vorhanden und das Präfix "neu" bedeutet: Links nicht vorhanden
			s_result = getValueFromIni("result");

			Stat stat1, stat2;

			string links, rechts; // Hier die Pfade zu links und rechts Ordner, d.h. zum Ordner 1 und Ordner 2
			getParent(links, rechts);

			ifstream ifstream_result(s_result, ios::in); // Hier stehen alle files die nicht vorhanden sind im linken bzw. rechten Ordner
			
			string copied_out_file = getValueFromIni("copied");  // Hier einfach dokumentieren welche files kopiert wurden 

			ofstream r(copied_out_file, ios::out | ios::app);

			string line;
			getline(ifstream_result, line);
			
			ifstream_result.clear(); // Brauchts falls Datei schon am Ende (hier eher unwahrscheinlich es den sie hätte nur 1 Eintrag
			ifstream_result.seekg(0, ios::beg); // Zurück an den Anfang

			

			while (getline(ifstream_result, line)) // Die Resultate durchsuchen nach "nicht vorhanden" und "neu" d.h. nicht vorhandeen in rechts undd neu in links
			{
				if (size_t pos = line.find(fehlt_links_prefix + ": ") != std::string::npos) // Ist rechts nicht vorhanden 
				{
					string file = MyStringUtil::trim(line.substr(pos + fehlt_links_prefix.length() + 1));
				
					string source_path = links + "/" + file; // fille links
					string dest_path =  rechts + "/" + file; // file rechts

					if (MyFileSystemUtil::is_directory(source_path)) {
						// Von Links nach Rechts kopieren
						MyFileSystemUtil::copy_directory(source_path, dest_path); 
					}
					else {
						MyFileSystemUtil::copy_file(source_path, dest_path);
					}

					++stat1.m_files_copied;
				}
				else if (line.find(fehlt_rechts_prefix + ": ") != std::string::npos) // Ist links nicht vorhanden
				{
					string file = MyStringUtil::trim(line.substr(pos + fehlt_rechts_prefix.length() + 1));
			
					string source_path = rechts + "/" + file; // file rechts
					string dest_path = links + "/" + file; // fille links
					
					if (MyFileSystemUtil::is_directory(source_path)) {
						// Von Links nach Rechts kopieren
						MyFileSystemUtil::copy_directory(source_path, dest_path);
					}
					else {
						MyFileSystemUtil::copy_file(source_path, dest_path);
					}

					++stat2.m_files_copied;
				}
				
			}

			return stat1.m_files_copied + stat2.m_files_copied;
		}

		static void executeMenu(void)
		{
			unsigned int selected;

			while (true)
			{
				try
				{
					std::this_thread::sleep_for(0.05s);

					cout << endl;
					cout << "Dateien auflisten: " << menu::MENU_LIST << endl;
					cout << "Dateien vergleichen: " << menu::MENU_COMPARE << endl;
					cout << endl;
					cout << "Relative path: " << (s_enabledRelative ? " |enabled| " : " |disabled| ") << " press " << menu::MENU_ENABLE_DISABLE_RELATIVE << " for change" << endl;
					cout << "Case sensitive: " << (s_caseSensitive ? " |enabled| " : " |disabled| ") << " press " << menu::MENU_ENABLE_DISABLE_CASE << " for change" << endl;
					cout << "Consider file size: " << (s_enabledFileSize ? " |enabled| " : " |disabled| ") << " press " << menu::MENU_ENABLE_DISABLE_SIZE << " for change" << endl;
					cout << endl;
					cout << "Beenden: " << menu::END << endl;

					cin >> selected;
					cin.ignore();

					switch (selected)
					{
					case menu::MENU_LIST:
					{
						listFiles();
					}
					break;
					case menu::MENU_COMPARE:
					{
						compareFiles();
					}
					break;
					case menu::MENU_ENABLE_DISABLE_RELATIVE:
						s_enabledRelative ^= 1;
						break;
					case menu::MENU_ENABLE_DISABLE_CASE:
						s_caseSensitive ^= 1;
						break;
					case menu::MENU_ENABLE_DISABLE_SIZE:
						s_enabledFileSize ^= 1;
						break;
					case menu::END:
						cout << endl << "Das Programm wird beendet." << endl;
						return;
						break;
					default:
						throw(exception("\nFalsche Eigabe!\n"));
						break;
					} // End Switch
				}
				catch (exception& e)
				{
					cout << endl << e.what() << endl;
				}
				catch (...)
				{
					cout << endl << "Unbekannter Fehler! Funktion: Main" << endl;
				}
			} // End while(true)
		}

		static void executeIni(const unsigned int cmd)
		{
			s_cycleCount = 0;
			switch (cmd)
			{
			case cmd::CMD_LIST:
			{
				listFiles();
				++s_cycleCount;
			}
			break;
			case cmd::CMD_COMPARE:
			{
				compareFiles();
				s_cycleCount = 0;
			}
			break;
			case cmd::CMD_ALL:
			{
				listFiles();
				++s_cycleCount;
				listFiles();
				compareFiles();
				s_cycleCount = 0;
			}
			break;
			case cmd::CMD_1_AND_2:
			{
				listFiles();
				++s_cycleCount;
				compareFiles();
				s_cycleCount = 0;
			}
			break;
			case cmd::CMD_SYNC:
			{
				listFiles();
				++s_cycleCount;
				listFiles();
				compareFiles();
				s_cycleCount = 0;
				
				sync_files();
			}
			case cmd::CMD_ONLY_SYNC:
			{
				s_cycleCount = 0;
				sync_files();
			}
			break;
			
			default:
				throw(exception("\nFalsche Eigabe!\n"));
				break;
			} // End Switch
		} //  executeIni(const unsigned int cmd)


		static void getParent(string& parent1, string& parent2)
		{ // Diese Funktion sucht den gemeinsamen Ordner von zwei Pfaden und schneidet beide 
			// so zurecht, dass als letzter Orndner im Pfad der gemeinsame Ordner ist
			try
			{
				string path1, path2, textFile1, textFile2;
				unsigned int dummyFileSize1, dummyFileSize2;
				
				textFile1 = getValueFromIni("fileList1");
				textFile2 = getValueFromIni("fileList2");

				ifstream f1(textFile1, ios::in);
				ifstream f2(textFile2, ios::in);

				getline(f1, path1);
				getline(f2, path2);

				if (s_enabledFileSize)
				{
					MyStringUtil::split(path1, path1, dummyFileSize1, WpFileSearch::s_separator);
					MyStringUtil::split(path2, path2, dummyFileSize2, WpFileSearch::s_separator);
				}

				MyStringUtil::trimWhiteSpaceRight(path1);
				MyStringUtil::trimWhiteSpaceRight(path2);

				bfs::path p1 = bfs::path(path1);
				bfs::path p2 = bfs::path(path2);

				parent1 = p1.parent_path().string();
				parent2 = p2.parent_path().string();

			}
			catch (...)
			{
				throw;
			}
		}

		static string getValueFromIni(const std::string& name)
		{
			try
			{
				ifstream file("ini.txt", ifstream::in);
				if (!file) { throw(std::exception("Kein gültiger Pfad zur ini-Datei! Funktion: SysUtil::rgetValueFromIni()")); }

				std::string line, _name, value;
				int posOfEq;

				while (getline(file, line))
				{
					//stringstream liness(line);
					//getline(liness, line);
					posOfEq = line.find("=");
					_name = line.substr(0, posOfEq);
					if (_name == name)
					{
						file.close();
						value = line.substr(posOfEq + 1, line.length());
						MyStringUtil::trimWhiteSpaceRight(value);
						return(value);
					}
				}
				file.close();
				return "";
			}
			catch (std::exception& e)
			{
				printf("\nError in SysUtil::getValueFromIni()\n");
				throw e;
			}
		}
		

		static void checkFile(string& path, const string fileName, const string ext = ".txt", const string separator = "/")
		{
			bfs::path p = bfs::path(path);
			if (p.empty()) // Wenn Pfad-String leer
			{
				throw(exception("Das ist kein Pfad sondern ein leerer String! Funtkion: main::checkFile()"));
			}

			if (bfs::is_directory(path)) // Wenn Ordner
			{
				path += (separator + fileName + ext);
			}
			else // Wenn Datei
			{
				if (!p.has_extension()) // Wenn keine Endung
				{// Endung hinzufügen
					path += ext;
					p = path;
				}
				else 
				{
					const string e(p.extension().string());
					if (!MyStringUtil::compareStringsWithSpace(e, ext))
					{
						string errorString = "\nFalsche Endung! Die Endung muss " + ext + " sein! Funktion: main::checkFile()\n";
						throw(exception(errorString.c_str()));
					}
				}
			}

			MyFileSystemUtil::normalizePath(path,s_caseSensitive);
			MyFileSystemUtil::create(path);
			MyFileSystemUtil::clearFile(path);
		}

		static void checkFile2(string& path, const string sep = "/")
		{
			bfs::path p = bfs::path(path);
			if (p.empty()) // Wenn Pfad-String leer
			{
				throw(exception("Das ist kein Pfad sondern ein leerer String! Funtkion: FileSearchUtil::checkFile2()"));
			}

			if (bfs::is_directory(path))
			{
				throw(exception("Das ist keine Datei sondern ein Ordner! Muss eine Datei sein! Funktion: FileSearchUtil::checkFile2()"));
			}

			if (!bfs::exists(path))
			{
				path += sep;
				if (!bfs::exists(path))
				{
					string errorString = "\nDie Datei existiert nicht! Funktion: FileSearchUtil::checkFile2()\n";
					throw(exception(errorString.c_str()));
				}
			}

			MyFileSystemUtil::normalizePath(path, s_caseSensitive);
		}


		static void checkDir(string& path, const string separator = "/")
		{
			bfs::path p = bfs::path(path);
			if (p.empty()) // Wenn Pfad-String leer
			{
				throw(exception("Das ist kein Pfad sondern ein leerer String! Funtkion: main::checkDir()"));
			}
			boost::system::error_code bsec;
			if (!bfs::is_directory(p, bsec)) // Wenn Pfad kein Ordner
			{
				string errorString = "\nDer Pfad muss ein Ordner sein! main::checkDir()\n";
				throw(exception(errorString.c_str()));
			}

			if (!bfs::exists(path)) // Wenn Der Ordner nicht existiert
			{
				// Dann einen Ordner anlegen
				MyFileSystemUtil::create_directory_from_file(path + separator);
			}

			MyFileSystemUtil::normalizePath(path, s_caseSensitive);
		}
	};

	Timer FileSearchUtil::s_timer;
	bool FileSearchUtil::s_enabledMenu;
	bool FileSearchUtil::s_enabledRelative;
	bool FileSearchUtil::s_caseSensitive;
	bool FileSearchUtil::s_enabledFileSize;
	unsigned int FileSearchUtil::s_cycleCount = 0;
	string FileSearchUtil::s_rootDir[2];
	string FileSearchUtil::s_fileList[2];
	string FileSearchUtil::s_standardFileName[2];
	string FileSearchUtil::s_standardDirName[2];
	string FileSearchUtil::s_result = "";
	string FileSearchUtil::s_ext = ".txt";
	Stat FileSearchUtil::s_stat;
	WpFileSearch FileSearchUtil::s_fs;
	

} // Namespace wp



#endif