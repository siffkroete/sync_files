#ifndef _MYSTRINGUTILH_
#define _MYSTRINGUTILH_

#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

namespace mystring
{
	class MyStringUtil
	{
	public:
		const static unsigned char AE = static_cast<unsigned char>(142);
		const static unsigned char ae = static_cast<unsigned char>(132);
		const static unsigned char OE = static_cast<unsigned char>(153);
		const static unsigned char oe = static_cast<unsigned char>(148);
		const static unsigned char UE = static_cast<unsigned char>(154);
		const static unsigned char ue = static_cast<unsigned char>(129);
		const static unsigned char ss = static_cast<unsigned char>(225);

		MyStringUtil()
		{
		}

		~MyStringUtil()
		{
		}

		// Schneidet alle leerzeichen am Ende des Strings weg und gibt die Anzahl
		// abgeschnittener leerzeichen zurück
		static unsigned int trimWhiteSpaceRight(string& s)
		{
			for (size_t i = s.size() - 1; i >= 0; --i)
			{
				if (s.c_str()[i] == ' ' || s.c_str()[i] == '\n')
				{
					s.erase(i, 1);
				}
				else
				{
					return s.size() - i -1;
				}
			}
		}

		static string trim(const std::string& s)
		{
			auto start = s.begin();
			while (start != s.end() && std::isspace(*start))
			{
				start++;
			}

			auto end = s.end();
			do {
				end--;
			} while (std::distance(start, end) > 0 && std::isspace(*end));

			return std::string(start, end + 1);
		}

		static bool compareStringsWithSpace(const string& s1, const string& s2)
		{
			if (s1.size() == s2.size())
			{
				return(s1 == s2);
			}
			else
			{
				string s1copy(s1), s2copy(s2);
				trimWhiteSpaceRight(s1copy);
				trimWhiteSpaceRight(s2copy);
				return(s1copy == s2copy);
			}
		}

		void slurp(ifstream& in, string& s)
		{
			try
			{
				stringstream sstr;
				sstr << in.rdbuf();
				s = sstr.str();
			}
			catch (...)
			{
				throw;
			}
		}

		static string mirroringString(string& s)
		{
			string _s = "";

			if (sizeof(s) > 255) { throw(exception("Der String ist zu lang! Funktion: MyStringUtil::mirroringString")); }

			string::reverse_iterator iter;

			for (iter = s.rbegin(); iter != s.rend(); iter++)
			{
				_s += *iter;
			}

			return _s;
		}

		static void outThousandGrouping(const string& s, const int destBasis)
		{
			int size = s.length();
			printf("\n");
			char res[64];

			int n = 0, z = 0, sep;

			if (destBasis == 2)
			{
				sep = 4;
			}
			else if (destBasis == 16)
			{
				sep = 2;
			}
			else
			{
				sep = 3;
			}

			while (z < size)
			{
				if (((size - z) % sep) == 0 && (z != 0)) { res[n++] = ' '; }
				res[n++] = s.c_str()[z++];
			}

			res[n] = '\0';
			printf(res);
			printf("\n");
		}

		static void split(const string& source, string& dest1, string& dest2, const char separator)
		{
			stringstream ss(source);
			getline(ss, dest1, separator);
			getline(ss, dest2);
		}

		static void split(const string source, string& dest1, unsigned int& dest2, const char separator)
		{
			string fileSize;
			split(source, dest1, fileSize, separator);
			dest2 = atoi(fileSize.c_str());
		}
	};
}
#endif
