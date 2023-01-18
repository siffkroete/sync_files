
#ifndef _LOG_
#define _LOG_

#include <iostream>
#include <fstream>
#include <sstream>

#include "MySingleton.h"

using namespace std;

namespace my_log
{
#define g_pLog Log::getSingleton()

	class Log : public MySingleton<Log>
	{
	public:

		~Log()
		{
		}

		void setPath(const string& path)
		{
			m_path = path;
		}

		void outLine(const string& outString)
		{
			try
			{
				ofstream ofs(m_path, ios::out | ios::app);
				ofs << outString << "\n";
			}
			catch(...)
			{
				throw;
			}
		}

		void outLine(const string& outString,const string& path)
		{
			try
			{
				ofstream ofs(path, ios::out | ios::app);
				ofs << outString << "\n";
			}
			catch (...)
			{
				throw;
			}
		}

	protected:
		string m_path;

		Log()
		{
		}
	};
}

#endif _LOG_