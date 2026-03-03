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
    public:
    static void even_loop(unsigned int core_id, udp_packet_t *w_offset) ;
};
#endif //MQTT_UDP_TRANSPORT