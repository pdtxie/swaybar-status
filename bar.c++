#include <iostream>
#include <fstream>

#include <format>

#include <ctime>
#include <cstdio>
#include <cmath>
#include <cstdint>

#include <string>
#include <unistd.h>

#include <sys/statvfs.h>

#include <chrono>
#include <thread>

using std::string;

string get_disk() {
	struct statvfs statvfs_sb {};

	auto ret = statvfs("/", &statvfs_sb);

	auto avail = statvfs_sb.f_bavail * statvfs_sb.f_bsize;
	auto total =  statvfs_sb.f_blocks * statvfs_sb.f_frsize;

	auto used = total - avail;

	double free_gb = (double) avail / 1024 / 1024 / 1024;
	float usage_p = (double) used / total * 100;
	char strbuf[100];
	snprintf(strbuf, sizeof(strbuf), "%.1fGiB free [used %.1f%%]", free_gb, usage_p);

	return string(strbuf);
}

string get_cpu() {
	static std::ifstream fin("/proc/stat");

	static uint64_t old_total = 0, old_idle = 0;

	uint64_t total = 0, idle;

	fin.clear();
	fin.seekg(0);

	uint64_t val;
	for (int i = 0; i < 11; i++) {
		if (i == 0) {
			std::string tmp; fin >> tmp;
			continue;
		}

		fin >> val;
		total += val;
		if (i == 4)
			idle = val;
	}


	auto d_total = total - old_total;
	auto d_idle = idle - old_idle;
	auto d_used = d_total - d_idle;

	auto usage = sysconf(_SC_NPROCESSORS_ONLN) * 100 * (double) d_used / d_total;

	old_total = total;
	old_idle = idle;

	char strbuf[40];
	snprintf(strbuf, sizeof(strbuf), "[%.1f%%]", usage);
	
	return string(strbuf);
}


string get_mem() {
	using std::ifstream; 
	using std::string;

	// kiB
	long long tot, free;
	string tmp;

	static ifstream buf("/proc/meminfo");
	if (!buf) return "err";

	buf.clear();
	buf.seekg(0);

	// don't even talk to me right now
	// TODO: fix
	buf >> tmp >> tot >> tmp >> tmp >> tmp >> tmp >> tmp >> free;

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

	// FIXME: custom icon patch
	
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

// INFO: conform to swaybar format
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

std::fstream fin("/proc/stat");

void print_json(std::ostream& os, std::vector<Widget> ws) {
	for (int i = 0; i < ws.size() - 1; i++)
		os << make_obj(ws[i]);
	
	os << make_obj(ws[ws.size() - 1], true);
}

int main() {
	using std::cout;
	// TODO: add click here
	cout << "{\"version\": 1}\n\n";
	cout << "[[]";

	// TODO: take input from click
	while (true) {
		cout << ",[";
		std::vector<Widget> ws = {
			{"disk", get_disk(), "#DBBDDBFF" },
			{"cpu", "| " + get_cpu() + " |", "#BDBDDBFF"},
			{"mem", get_mem(), "#BDDBDBFF"},
			{"date", "| " + get_td().first + " |"},
			{"time", get_td().second, "#FFFFFFFF"}
		};

		print_json(cout, ws);

		cout << "]" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}
