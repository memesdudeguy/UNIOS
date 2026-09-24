#include "process.h"

static struct unios_process process_table[UNIOS_MAX_PROCESSES];
static uint32_t process_count = 0U;

void process_init(void) {
    process_count = 0U;
    process_table[0].pid = 1U;
    process_table[0].state = UNIOS_PROC_RUNNING;
    process_table[0].entry = 0U;
    process_table[0].stack_top = 0x200000U;
    process_table[0].pages = 1U;
    process_table[0].name[0] = 'k';
    process_table[0].name[1] = 'e';
    process_table[0].name[2] = 'r';
    process_table[0].name[3] = 'n';
    process_table[0].name[4] = 'e';
    process_table[0].name[5] = 'l';
    process_table[0].name[6] = '\0';
    process_count = 1U;
}

struct unios_process *process_add(const char *name, uint32_t entry) {
    uint32_t i;
    if (process_count >= UNIOS_MAX_PROCESSES) {
        return 0;
    }
    i = process_count;
    process_table[i].pid = (uint32_t)(1000U + i);
    process_table[i].state = UNIOS_PROC_RUNNING;
    process_table[i].entry = entry;
    process_table[i].stack_top = 0x300000U + (i * 0x1000U);
    process_table[i].pages = 1U;
    for (uint32_t j = 0; j < 15U; ++j) {
        process_table[i].name[j] = 0;
    }
    if (name) {
        for (uint32_t j = 0; j < 15U; ++j) {
            process_table[i].name[j] = name[j];
            if (name[j] == '\0') {
                break;
            }
        }
    }
    process_count++;
    return &process_table[i];
}

int process_kill_pid(uint32_t pid) {
    for (uint32_t i = 0; i < process_count; ++i) {
        if (process_table[i].pid == pid) {
            process_table[i].state = UNIOS_PROC_ZOMBIE;
            return 0;
        }
    }
    return -1;
}
