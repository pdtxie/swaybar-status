#include <iostream>
#include <ctime>
#include <cstdio>
#include <cmath>

#include <fstream>

#include <chrono>
#include <thread>

using std::string;
string get_mem() {
	using std::ifstream; 
	using std::string;

	// kB
	long long tot, free;
	string tmp;

	ifstream buf("/proc/meminfo");
	if (!buf) return "err";
	buf >> tmp >> tot >> tmp >> tmp >> tmp >> tmp >> tmp >> free;
	buf.close();

	float tot_gb = static_cast<float>(tot)/(1024*1024), free_gb = static_cast<float>(free)/(1024*1024);
	float used_gb = tot_gb - free_gb;
	int used_percent = static_cast<int>(100 * (tot - free) / tot);

	char strbuf[40];
	snprintf(strbuf, sizeof(strbuf), "%.2f/%.2fGiB [%i%%]", used_gb, tot_gb, used_percent);
	
	return string(strbuf);
}

std::pair<string, string> get_td() {
	std::time_t t = std::time(nullptr);

	char tbuf[25], dbuf[25];

	struct tm* lct = localtime(&t);

	// todo custom icon patch
	/* std::string icon;
	switch (lct->tm_hour % 12) {
		case 0: { icon = ""; break; }
		case 1: { icon = ""; break; }
		default: icon = 'a';
	} */

	strftime(dbuf, sizeof(dbuf), "%a, %d %b %Y", lct);
	strftime(tbuf, sizeof(tbuf), "%I:%M:%S %p ", lct);

	return std::make_pair(string(dbuf), string(tbuf));
}


struct Widget {
	string name, full_text, colour="#D9D9D9FF";
};

string make_obj(Widget w, bool end=false) {
	char buf[150];
	snprintf(buf, sizeof(buf), "{\"name\": \"%s\", \
								\"full_text\": \"%s\", \
								\"separator\": \"false\", \
								\"color\": \"%s\"}%c",
			 w.name.c_str(), w.full_text.c_str(), w.colour.c_str(), end ? ' ': ',');
	return string(buf);
}

void print_json(std::ostream& os, std::vector<Widget> ws) {
	for (int i = 0; i < ws.size() - 1; i++)
		os << make_obj(ws[i]);
	
	os << make_obj(ws[ws.size() - 1], true);
}

int main() {
	using std::cout;
	cout << "{\"version\": 1}\n\n";
	cout << "[[]";


	while (true) {
		cout << ",[";
	
		std::vector<Widget> ws = {
			{"mem", get_mem(), "#DBBDDBFF"},
			{"date", "| " + get_td().first + " |"},
			{"time", get_td().second, "#FFFFFFFF"}
		};

		print_json(cout, ws);

		cout << "]" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
