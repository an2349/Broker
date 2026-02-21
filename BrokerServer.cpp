//
// Created by an on 2/14/26.
//
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <cstring>
#include <span>
#include <thread>
#include <unordered_map>
#include <vector>
#include "config.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


using namespace std;


class BrokerServer {
private:
    thread ServerThread;
    int serverFd = -1;
    int epfd = -1;
    int notifyFd = -1;

    struct ClientInfo {
        string ClientMac;
        string ClientIp;
        uint8_t ClientRequestType;
        time_t lastActive;
        uint16_t topicLen;
        string topicName;
        uint16_t contentLen;
        int requestCount = 0;
        bool isAuth = false;
        vector<uint8_t> *byteRead;
    };

    unordered_map<int, ClientInfo*> clientMap;

    struct raw_mqttPacket {//tro vao buffer khi doc tcp
        uint8_t  type;//1byte
        uint16_t length; //nhieu byte neu co
        uint16_t topicLen; //byte cua ten topic neu co
        string_view topicName; //tro vao buffer neu co
        span<uint8_t> payload;//tro vao buffer neu co
    };
    struct mqttPacket {//lay tu clientinfo ra khi doc xong tu tcp
        uint8_t  type;
        uint32_t length;
        uint16_t topicLen;
        string_view topicName;
        vector<uint8_t> payload;
    };





    void evenLoop(unsigned int n);
    void spawnWorker(unsigned int n);

public:
    void start(unsigned int n);
    void stop();
    void restart();
protected:
    unsigned int getRequest();
};

void BrokerServer::start(unsigned int n) {
        struct sockaddr_in address; //ipv4
        int addrlen = sizeof(address);

        if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            cerr << " loi o socket\n";
            return;
        }
        int opt = 1;
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            cerr << "setsockopt SO_REUSEADDR failed\n";
        }
#ifdef SO_REUSEPORT
        if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            cerr << "setsockopt SO_REUSEPORT failed\n";
        }
#endif

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(serverFd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            cerr << "loi o bind port\n";
            return;
        }

        if (listen(serverFd, MAX_CONN) < 0) {
            cerr << " loi lang nghe\n";
            return;
        }

        /*struct epoll_event ev{};
        struct epoll_event event[MAX_EVENTS];

        ev.events = EPOLLIN;
        ev.data.fd = notifyFd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, notifyFd, &ev);

        ev.events = EPOLLIN;
        ev.data.fd = serverFd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, serverFd, &ev);*/

    for (unsigned int i = 0; i < n; ++i) {
        spawnWorker(i);
    }
    while (true) {
        int status;
        pid_t dead_pid = wait(&status);
        if (dead_pid > 0) {
            spawnWorker(0);
        }
    }
}

void BrokerServer::evenLoop(unsigned int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id % std::thread::hardware_concurrency(), &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int local_epfd = epoll_create1(0);
    struct epoll_event ev{};
    struct  epoll_event events[MAX_EVENTS];


    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = serverFd;
    epoll_ctl(local_epfd, EPOLL_CTL_ADD, serverFd, &ev);

    while (true) {
        int nfds = epoll_wait(local_epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == serverFd) {

            } else {

            }
        }
    }
}

void BrokerServer::spawnWorker(unsigned int id) {
    pid_t pid = fork();
    if (pid == 0) {
        evenLoop(id);
        exit(0);
    } else if (pid < 0) {
    }
}