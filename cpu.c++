// user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>

#include <unistd.h>

#include <chrono>
#include <thread>

int main() {
    std::ifstream fin("/proc/stat");

    uint64_t old_total = 0, old_idle = 0;

    while (1) {
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
        std::cout << usage << std::endl;

        old_total = total;
        old_idle = idle;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}
