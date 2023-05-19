#include <iostream>
#include <ctime>
#include <cstdio>
#include <cmath>

#include <fstream>

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

string get_date() {
	std::time_t t = std::time(nullptr);

	char buf[50];
	struct tm* lct = localtime(&t);

	// todo custom icon patch
	/* std::string icon;
	switch (lct->tm_hour % 12) {
		case 0: { icon = ""; break; }
		case 1: { icon = ""; break; }
		default: icon = 'a';
	} */
	strftime(buf, sizeof(buf), "%a, %d %b %Y | %I:%M:%S %p ", lct);

	return string(buf);
}

int main() {
	using std::cout;
	cout << get_mem() << " | " << get_date();
	return 0;
}
