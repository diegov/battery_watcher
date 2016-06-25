#include <libnotify/notify.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <climits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void notify_send(string msg) {
    NotifyNotification *n;
    n = notify_notification_new("Battery status", msg.c_str(), NULL);
    notify_notification_set_timeout(n, 10000);
    if (!notify_notification_show (n, NULL)) {
        syslog(LOG_ERR, "Failed to send notification!");
    }
    g_object_unref(G_OBJECT(n));
}

class Watcher {
public:
    Watcher(string path);
    void checkStatus();
private:
    string path;
    int last_status;
};

Watcher::Watcher(string path) : path(path), last_status(INT_MAX) { }

void Watcher::checkStatus() {
    ifstream file(this->path, ios::binary | ios::ate);
    streamsize size = file.tellg();    
    if (size >= 0) {
        file.seekg(0, ios::beg);

        stringstream buffer;
        buffer << file.rdbuf();

        string percent_str = buffer.str();
        try {
            int percent = stoi(percent_str);
            if (this->last_status > 10 && percent <= 10) {
                notify_send("Battery charge under 10%");
            }
            else if (this->last_status > 20 && percent <= 20) {
                notify_send("Battery charge under 20%");
            }
            this->last_status = percent;
        } catch (std::invalid_argument e) {
            string msg = "Failed to parse ";
            msg.append(percent_str.substr(0, 100));
            syslog(LOG_ERR, msg.c_str());
        }
    }
    file.close();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " power-device-path" << endl;
        exit(EXIT_FAILURE);
    }

    string device_path = argv[1];

    const char *name = "battery-watch";

    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(name, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

    pid_t pid, sid;

    pid = fork();

    if (pid < 0) { exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }

    umask(0);

    sid = setsid();
    if (sid < 0) { exit(EXIT_FAILURE); }

    if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    string msg = "Monitoring ";
    msg.append(device_path);

    syslog(LOG_INFO, msg.c_str());

    notify_init(name);

    Watcher watcher(device_path);
 
    while (true) {
        watcher.checkStatus();
        sleep(10);
    }

    closelog();
}
