#ifndef UNIOS_ORBITLINE_H
#define UNIOS_ORBITLINE_H

#define ORBITLINE_MAX_INPUT 256
#define ORBITLINE_HISTORY_SIZE 64

struct orbitline_io {
    int  (*read_key)(void);
    void (*write_char)(char c);
    void (*write_text)(const char *text);
    void (*move_cursor_left)(unsigned count);
    void (*move_cursor_right)(unsigned count);
    void (*erase_chars)(unsigned count);
    void (*clear_screen)(void);
};

struct orbitline {
    char buffer[ORBITLINE_MAX_INPUT];
    unsigned length;
    unsigned cursor;

    char history[ORBITLINE_HISTORY_SIZE][ORBITLINE_MAX_INPUT];
    unsigned history_count;
    int history_position;

    const struct orbitline_io *io;
};

void orbitline_init(
    struct orbitline *line,
    const struct orbitline_io *io
);

int orbitline_read(
    struct orbitline *line,
    char *output,
    unsigned output_size
);

void orbitline_add_history(
    struct orbitline *line,
    const char *command
);

#endif
