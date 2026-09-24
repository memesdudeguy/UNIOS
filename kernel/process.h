#ifndef UNIOS_PROCESS_H
#define UNIOS_PROCESS_H

#include <stdint.h>

#define UNIOS_MAX_PROCESSES 8U
#define UNIOS_PROC_RUNNING 1U
#define UNIOS_PROC_STOPPED 2U
#define UNIOS_PROC_ZOMBIE 3U

struct unios_process {
    uint32_t pid;
    char name[16];
    uint32_t state;
    uint32_t entry;
    uint32_t stack_top;
    uint32_t pages;
};

void process_init(void);
struct unios_process *process_add(const char *name, uint32_t entry);
int process_kill_pid(uint32_t pid);

#endif
