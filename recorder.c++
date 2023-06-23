#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include <iostream>
#include <optional>

int is_recording() {
    FILE* fp;
    fp = popen("pidof -s wf-recorder", "r");

    char buf[100];
    fgets(buf, 100, fp);
    pid_t pid = strtoul(buf, NULL, 10);

    // pclose(fp);
    if (pid == 0) return -1;
    return pid;
}

void stop_recording(pid_t pid) {
    kill(pid, SIGINT);
}

int main() {
    std::cout << is_recording() << std::endl;

    pid_t p = is_recording();
    if (p != -1) stop_recording(p);
    return 0;
}
