#ifndef UNIOS_ORBITCLI_H
#define UNIOS_ORBITCLI_H

struct orbit_command {
    const char *name;
    const char *action;
    const char *description;
};

void orbitcli_init(void);
void orbitcli_print_prompt(void);
int orbitcli_execute(const char *line);

#endif
