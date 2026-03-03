//
// Created by an on 3/3/26.
//
#include "Workers.h"




void  Workers::even_loop(unsigned int core_id, udp_packet_t *w_offset) {
        cpu_set_t cpuset;
        int worker_fd = -1;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id % std::thread::hardware_concurrency(), &cpuset);
        sched_setaffinity(0, sizeof(cpuset), &cpuset);
        struct sockaddr_in address; //ipv4
        int addrlen = sizeof(address);

        if ((worker_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            //if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            std::cerr << " loi o socket\n";
            return;
        }
        int opt = 1;
        if (setsockopt(worker_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "setsockopt SO_REUSEADDR failed\n";
        }
#ifdef SO_REUSEPORT
        if (setsockopt(worker_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            std::cerr << "setsockopt SO_REUSEPORT failed\n";
        }
#endif

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(worker_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
            std::cerr << "loi o bind port\n";
            return;
        }

        /*if (listen(serverFd, MAX_CONN) < 0) {
            cerr << " loi lang nghe\n";
            return;
        }*/
        fcntl(worker_fd, F_SETFL, fcntl(worker_fd, F_GETFL, 0) | O_NONBLOCK);

        struct io_uring ring;
        io_uring_queue_init(MAX_PACKAGE, &ring, IORING_SETUP_SQPOLL);

        struct iovec iov;
        iov.iov_base = w_offset;
        iov.iov_len  = MAX_PACKAGE * sizeof(udp_packet_t);

        io_uring_register_buffers(&ring, &iov, 1);

        struct io_uring_sqe *sqe;
        sqe = io_uring_get_sqe(&ring);
        io_uring_prep_provide_buffers(sqe, w_offset, sizeof(udp_packet_t), MAX_PACKAGE, core_id, 0);
        io_uring_submit(&ring);
    }
