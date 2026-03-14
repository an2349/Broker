//
// Created by an on 3/3/26.
//

#include "Mem_manager.h"

#include <iostream>

udp_packet_t* Mem_manager::setup_mem_package_pool(Mem_manager& mm, size_t pool_size) {
    std::cout<<"Creating hp...\n";
    void *ptr = nullptr;
    while (mm.retry < 2) {
        ptr = mmap(NULL, pool_size,
                         PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_POPULATE | MAP_SHARED | MAP_HUGETLB,
                         -1, 0);

        if (ptr != MAP_FAILED) {
            std::cout<<"Success\n";
            return static_cast<udp_packet_t *>(ptr);
        }
        else{
            mm.retry++;
            if (!request_hugepages((pool_size + 2 * 1024 * 1024 - 1) >> 21)) {
                std::cout<< "HP fail "<< mm.retry+1 << std::endl;
                break;
            }
        }
    }
            perror("Cannot hugepage,use default page");
            ptr = mmap(NULL, pool_size,
                       PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_POPULATE | MAP_SHARED,
                       -1, 0);

            if (ptr == MAP_FAILED) {
                perror("Cannot mmap");
                return nullptr;
            }
        //madvise(ptr, pool_size, MADV_WILLNEED | MADV_SEQUENTIAL);
        //memset(ptr, 0, pool_size);
    }



bool Mem_manager::request_hugepages(int num_pages) {
    std::ofstream hp_file("/proc/sys/vm/nr_hugepages");
    if (!hp_file.is_open()) {
        perror("Run as sudo!");
        return false;
    };
    hp_file << num_pages;
    hp_file.close();
    std::ifstream check_file("/proc/sys/vm/nr_hugepages");
    int actual_pages;
    check_file >> actual_pages;

    if (actual_pages < num_pages) {
        std::cout<<"max page : " << actual_pages << std::endl;
    }
    return true;
}
