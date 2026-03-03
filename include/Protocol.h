//
// Created by an on 3/3/26.
//

#ifndef BROKER_PROTOCOL_H
#define BROKER_PROTOCOL_H
#include <cstdint>

struct connection_stream {
};

struct connection_state {
    // bit 0: is_auth
    // bit 1: is_encrypted
    // bit 2-7: reserved
    uint8_t flags; // 1 byte for Auth
    uint32_t conn_id; // 4 bytes
    uint32_t stream_id; //4
    uint16_t topic_len; // 2 bytes
    uint16_t last_active; // 2 bytes
    uint32_t package_number; // 4 bytes
    uint8_t request_type; // 1 byte
    uint16_t index; //2
} __attribute__((aligned(32)));

struct raw_mqtt_packet {
    uint8_t type;
    uint16_t length;
    uint16_t topicLen;
    string_view topicName;
    span<uint8_t> payload;
};

struct mqtt_packet {
    uint8_t type;
    uint32_t length;
    uint16_t topicLen;
    string_view topicName;
    span<uint8_t> payload;
};

typedef struct {
    uint32_t conn_id; // 4
    uint32_t src_ip; // 4
    uint16_t src_port; // 2
    uint16_t packet_len; // 2
    uint64_t seq_num; // 8
    uint8_t status; // 1 (0: EMPTY, 1: READY, 2: WORKING)
    uint8_t padding[43]; // Padding
    uint8_t payload[1984]; // more
} __attribute__((aligned(64))) udp_packet_t;


#endif //BROKER_PROTOCOL_H