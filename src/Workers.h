//
// Created by an on 3/3/26.
//

#ifndef MQTT_UDP_TRANSPORT_WORKERS_H
#define MQTT_UDP_TRANSPORT_WORKERS_H

#include <fcntl.h>
#include <iostream>
#include <liburing.h>
#include <sched.h>
#include <thread>
#include <bits/cpu-set.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "config.h"
#include "../include/Protocol.h"

class Workers {
private:
    int worker_fd;
    struct io_uring ring;
    unsigned int core_id;
    udp_packet_t * w_offset;

    void setup_any();

    void setup_socket();

public:
    Workers(unsigned int id, udp_packet_t * w_offset);

    //  static void even_loop(unsigned int core_id, udp_packet_t *w_offset) ;

    //~Worker();

    //Worker(const Workers &) = delete;

    //Workers &operator=(const Workers &) = delete;

    void init_uring();

    void working();
};
#endif //MQTT_UDP_TRANSPORT
