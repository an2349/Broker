//
// Created by an on 3/3/26.
//

#include "Mem_manager.h"

udp_packet_t* Mem_manager::setup_mem_package_pool(size_t pool_size) {
        void *ptr = mmap(NULL, pool_size,
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_POPULATE | MAP_SHARED | MAP_HUGETLB,
                         -1, 0);

        if (ptr == MAP_FAILED) {
            perror("Cannot hugepage,use default page");
            ptr = mmap(NULL, pool_size,
                       PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_POPULATE | MAP_SHARED,
                       -1, 0);

            if (ptr == MAP_FAILED) {
                perror("Cannot mmap");
                return nullptr;
            }
        }
        //madvise(ptr, pool_size, MADV_WILLNEED | MADV_SEQUENTIAL);
        //memset(ptr, 0, pool_size);

        return static_cast<udp_packet_t *>(ptr);
    }
