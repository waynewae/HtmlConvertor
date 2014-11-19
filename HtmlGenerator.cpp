#include<iostream>
#include<fstream>
#include<string>
#include<cstring>

using namespace std;

void WriteHeader(fstream &WriteToHere)
{
	WriteToHere << "<!doctype html>\n"
				<< "<html>\n<head>\n"
				<< "<meta http-equiv=Content-Type content=\"text/html; charset=utf8\">"
				<< "<style type=\"text/css\">\n"
				<< "body{font-family:Arial;background-color:#D2DCDD;}\n"
				<< "h2{background-color:#5A675D;color:white;}\n"
				<< "table{border-style:double;background-color:white;}\n"
				<< "td{border-width:1px;border-style:solid;}\n"
				<< "div{font-size:16px;}\n"
				<< "#tilte{font-weight:bolder;background-color:#DED9D5;}\n"
				<< "#battery{text-align:center;}\n"
				<< "</style>\n"
				<< "</head>\n"
				<< "<body>\n";
}

int main(int argc, char* argv[])
{
	char line[100] = {0};
	string tmp;
	string logfile = "DoU_AutoTest_report.txt";
	int stage = 0, error_count = 0, abnormal_count = 0;

	// create DoU_AutoTest_report.html
	fstream WriteToHere;
	WriteToHere.open("DoU_AutoTest_report.html", ios::out | ios::trunc);
	if(!WriteToHere) cout << "Open DoU_AutoTest_report.html failed.\n";
	else cout << "Open DoU_AutoTest_report.html successful.\n\n";

	// write const format of html
	WriteHeader(WriteToHere);

	// open DoU_AutoTest_report.txt
	fstream log;
	log.open(logfile.c_str(), ios::in);
	if(!log) cout << "Open DoU_AutoTest_report.html failed.\n";
	else cout << "Open DoU_AutoTest_report.html successful.\n\n";

	// count error & abnormal
	while(log.getline(line, sizeof(line), '\n'))
	{
		tmp.assign(line);
		if(!tmp.compare("Error of AutoTest script"))
			while(log.getline(line, sizeof(line), '\n'))
			{
				tmp.assign(line);
				if(!tmp.compare(0, 4, "fail")) error_count++;
				if(!tmp.compare("Abnormal power consumption")) break;
			}
		if(!tmp.compare(0, 5, "Start")) abnormal_count++;
	}
	log.close();
//	cout << error_count << endl << abnormal_count << endl;


	// open DoU_AutoTest_report.txt
	log.open(logfile.c_str(), ios::in);
	if(!log) cout << "Open DoU_AutoTest_report.html failed.\n";
	else cout << "Open DoU_AutoTest_report.html successful.\n\n";

	while(log.getline(line, sizeof(line), '\n'))
	{
		tmp.assign(line);
		if(stage == 0)
		{
			// write Test Info
			if(!tmp.compare("Information"))
			{
				WriteToHere << "<h2>Test Information</h2>\n<div>\n";
				while(log.getline(line, sizeof(line), '\n'))
				{
					tmp.assign(line);
					if(tmp.compare("Battery Status")) WriteToHere << tmp << "<br/>\n";
					else
					{
						WriteToHere << "AutoTest Error: " << error_count << "<br/>\n"
									// abnormal_count -1 to remove start of battery status
									<< "Abnormal Power Consumption: " << abnormal_count -1 << "<br/>\n"
									<< "</div>\n";
						stage++;
						break;
					}
				}
			}
		}

		// write battery
		if(stage == 1)
		{
			WriteToHere << "<h2>Battery Status</h2>\n<div>\n<table id=\"battery\">\n"
						<< "<tr id=\"tilte\">\n<td>Time<br/></td>\n<td>Capacity(\%)<br/></td>\n"
						<< "<td>Charge full(mAh)<br/></td>\n<td>Charge now(mAh)<br/></td>\n</tr>\n";
			while(log.getline(line, sizeof(line), '\n'))
			{
				tmp.assign(line);
				if(tmp.compare("Error of AutoTest script"))
				{
					if(!tmp.compare(0, tmp.find(','), "Start")) WriteToHere << "<tr>\n<td>" << tmp << "<br/></td>\n";
					if(!tmp.compare(0, tmp.find(','), "Capacity")) WriteToHere << "<td>" << tmp.substr(tmp.find(',')+1, tmp.length()-tmp.find(',')) << "</td>\n";
					if(!tmp.compare(0, tmp.find(','), "Charge full")) WriteToHere << "<td>" << tmp.substr(tmp.find(',')+1, tmp.length()-1) << "</td>\n";
					if(!tmp.compare(0, tmp.find(','), "Charge now")) WriteToHere << "<td>" << tmp.substr(tmp.find(',')+1, tmp.length()-1) << "</td>\n</tr>\n";
					if(!tmp.compare(0, tmp.find(','), "End")) WriteToHere << "<td>" << tmp << "</td>\n";
				}
				else
				{
					WriteToHere << "</table>\n</div>\n";
					stage++;
					break;
				}
			}
		}

		//write Error of AutoTest script
		if(stage == 2)
		{
			WriteToHere << "<h2>AutoTest Error</h2>\n<div>\n";
			if(error_count)
			{
				while(log.getline(line, sizeof(line), '\n'))
				{
					tmp.assign(line);
					if(tmp.compare("Abnormal power consumption")) WriteToHere << tmp << "<br/>\n";
					else
					{
						stage++;
						break;
					}
				}
			}
			else
			{
				WriteToHere << "N/A<br/>\n";
				stage++;
			}
			WriteToHere << "</div>\n";
		}

		// write wakelock info
		if(stage == 3)
		{
			bool first = 1;
			WriteToHere << "<h2>Abnormal Power Consumption</h2>\n<div>\n";
			if(error_count)
			{
				while(log.getline(line, sizeof(line), '\n'))
				{
					tmp.assign(line);
					if(!tmp.compare(0, tmp.find(','), "Start"))
					{
						if(!first)
						{
							WriteToHere << "</table>\n"
										<< "<hr color=\"gray\">";
						}
						WriteToHere << "<b>" << tmp << "</b><br/>\n";
					}
					else if(!tmp.compare(0, tmp.find(','), "End")) WriteToHere << "<b>" << tmp << "</b><br/>\n";
					else if(!tmp.compare("User wakelocks") || !tmp.compare("Kernel wakelocks"))
					{
						if(!tmp.compare("Kernel wakelocks") && !first) WriteToHere << "</table>\n";
						WriteToHere << "<b>" << tmp << "</b><br/>\n<table>\n"
									<< "<tr id=\"tilte\">\n<td>Application<br/></td>\n<td>time<br/></td>\n</tr>\n";
						first = 0;
					}
					else
					{
						tmp.assign(line);
						WriteToHere << "<tr>\n<td>" << tmp.substr(0, tmp.find(','))
									<< "</td>\n<td>" << tmp.substr(tmp.find(',')+1, tmp.length())
									<< "</td>\n</tr>\n";
					}
				}
			}
			else
			{
				WriteToHere << "N/A<br/>\n";
				stage++;
			}
			WriteToHere << "</div>\n";
		}
	}
	WriteToHere << "</body>\n</html>\n";
	WriteToHere.close();
	log.close();
	return 0;
}
