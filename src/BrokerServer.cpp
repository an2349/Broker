//
// Created by an on 2/14/26.
//
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <span>
#include "../include/config.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <liburing.h>
#include <thread>

#include "Workers.h"
#include "../include/Protocol.h"

using namespace std;


class BrokerServer {
private:
    int epfd = -1;
    int notifyFd = -1;
    udp_packet_t *pool = nullptr;

    void spawn_worker(unsigned int core_id,  pid_t *workers_pid,  udp_packet_t *w_offset);

public:
    void start(unsigned int n);

    void stop();

    void restart();

protected:
    unsigned int getRequest();
};

void BrokerServer::start(unsigned int n) {
    //tao be de du lieu udp do vao
    size_t pool_size = (size_t)n *  MAX_PACKAGE * sizeof(udp_packet_t);
    pool = Mem_manager::setup_mem_package_pool(pool_size);
    if (!pool) {
        exit(1);
    }

    pid_t workers[n];
    for (unsigned int core_id = 0; core_id < n; ++core_id) {
        udp_packet_t* w_offset = pool + (core_id * MAX_PACKAGE);
       spawn_worker(core_id, workers, w_offset);
    }
    while (true) {
        int status;
        pid_t dead_pid = wait(&status);
        if (dead_pid > 0) {
            for (unsigned int i = 0; i < n; ++i) {
                if (workers[i] == dead_pid) {
                    udp_packet_t* w_offset = pool + (i * MAX_PACKAGE);
                    spawn_worker(i, workers , w_offset);
                    break;
                }
            }
        }
    }
}



void BrokerServer::spawn_worker(unsigned int core_id, pid_t *workers_pid, udp_packet_t *w_offset) {
    pid_t pid = fork();
    if (pid == 0) {
        Workers::even_loop(core_id, w_offset);
        exit(0);
    } else if (pid > 0) {
        workers_pid[core_id] = pid;
    } else {
        cerr<<"False in spawn worker "<<core_id;
    }
}


