#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "FileSearchUtil.h"
#include "WpFileSearch.h"

using namespace std;
using namespace wp;


/*
* Boost habe ich folgendermassen zum Laufen gebracht:
* 
* 1.) https://www.boost.org/doc/libs/1_79_0/more/getting_started/windows.html hier die zip f�r windows runterladen
* 2.) In irgendeinem Ordner speichern (bei mir C:/Homepage/)
* 3.) Auf bootstrap.bat (innterhalb des boost_1_79_0 Ordners dr�cken)
* 4.) Auf b2.exe dr�ckhen, dann kam ganz lange compilierung.
* 5.) Dann auf Linker General und Zus�tzliche lib Dateien und dann den Lib Ordner im boost_1_79_0/stage/lib  hinzuf�gen
* 6.) Und nat�rlich bei C++ General oder so den include Pfad zum boost_1_57_0/ Ordner hinzuf�gen.
* 
* 
* 
* Hier noch die Tips in Stackoverflow:
* 
* Go to were the extracted file was c:\boost_1_57_0. (Ich hab halt die neuere Version).
Click on booststrap.bat (don't bother to type on the command window just wait and don't close the window that is the place I had my problem that took me two weeks to solve. 
After a while the booststrap will run and produce the same file, but now with two different names: b2, and bjam.
Click on b2 and wait it to run.
Click on bjam and wait it to run. Then a folder will be produce called stage.
Right click on the project.
Click on property.
Click on linker.
Click on general.
Click on include additional library directory.
Select the part of the library e.g. c:\boost_1_57_0\stage\lib.
And you are good to go!
*/


int main()
{
	int returnValue(0);

	try
	{

		unsigned int cmd = atoi(FileSearchUtil::getValueFromIni("cmd").c_str());

		FileSearchUtil::init();

		if (cmd == FileSearchUtil::cmd::CMD_MENU)
		{
			FileSearchUtil::executeMenu();
			return 0;
		}
		else
		{
			FileSearchUtil::executeIni(cmd);

		}
	}
	catch (exception& e)
	{
		cout << endl << e.what() << endl;
		cout << endl << "Das Programm wird beendet." << endl;
		returnValue = -1;
	}
	catch (...)
	{
		cout << endl << "Unbekannter Fehler! Funktion: Main" << endl;
		cout << endl << "Das Programm wird beendet." << endl;
		returnValue = -1;
	}

	if (FileSearchUtil::getValueFromIni("endByEnter") == "enabled" || FileSearchUtil::getValueFromIni("endByEnter") == "true")
	{ // Wenn endByEnter in der Ini-Datei aktiviert dann muss der Benutzer Enter dr�cken um das Programm zu beenden
		cout << endl << "Zum Beenden dr" << MyStringUtil::ue << "cken Sie Enter" << endl;
		cin.get();
	}

	return returnValue;


}