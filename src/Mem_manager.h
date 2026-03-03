//
// Created by an on 3/3/26.
//

#ifndef MQTT_UDP_TRANSPORT_MEM_MANAGER_H
#define MQTT_UDP_TRANSPORT_MEM_MANAGER_H

#include <cstdio>
#include <cstring>
#include <sys/mman.h>

#include "../include/Protocol.h"

class Mem_manager {
public:
    static udp_packet_t *setup_mem_package_pool(size_t pool_size);
};
#endif //MQTT_UDP_TRANSPORT_MEM_MANAGER_H
