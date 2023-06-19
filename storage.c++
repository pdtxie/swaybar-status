#include <sys/statvfs.h>
#include <iostream>
#include <cstring>



int main() {
    struct statvfs statvfs_sb {};

    auto ret = statvfs("/", &statvfs_sb);

    auto avail = statvfs_sb.f_bavail * statvfs_sb.f_bsize;
    auto total =  statvfs_sb.f_blocks * statvfs_sb.f_frsize;

    auto used = total - avail;

    std::cout << "available: " << avail / 1024 / 1024 / 1024 << "GiB\n";
    std::cout << (double) used / total << std::endl;

    return 0;
}
