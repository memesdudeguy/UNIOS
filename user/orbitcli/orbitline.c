#include "orbitline.h"

#define KEY_BACKSPACE 8
#define KEY_TAB       9
#define KEY_ENTER     13
#define KEY_ESC        27
#define KEY_CTRL_C    3
#define KEY_CTRL_L    12
#define KEY_CTRL_U    21
#define KEY_CTRL_W    23
#define KEY_UP        1001
#define KEY_DOWN      1002
#define KEY_LEFT      1003
#define KEY_RIGHT     1004
#define KEY_HOME      1005
#define KEY_END       1006
#define KEY_DELETE    1007

static void redraw_from_cursor(struct orbitline *line)
{
    if (!line || !line->io)
        return;

    line->io->write_text("\r");
    line->io->write_text("[UNIOS::ORBIT] pilot@ship:/ > ");
    line->io->write_text(line->buffer);

    line->io->write_text(" ");

    unsigned end_distance =
        line->length + 1 - line->cursor;

    if (end_distance && line->io->move_cursor_left)
        line->io->move_cursor_left(end_distance);
}

static void clear_line(struct orbitline *line)
{
    if (!line)
        return;

    line->length = 0;
    line->cursor = 0;
    line->buffer[0] = 0;

    redraw_from_cursor(line);
}

static void insert_char(struct orbitline *line, char c)
{
    if (!line || line->length >= ORBITLINE_MAX_INPUT - 1)
        return;

    for (unsigned i = line->length; i > line->cursor; i--)
        line->buffer[i] = line->buffer[i - 1];

    line->buffer[line->cursor] = c;
    line->length++;
    line->cursor++;
    line->buffer[line->length] = 0;

    redraw_from_cursor(line);
}

static void backspace(struct orbitline *line)
{
    if (!line || line->cursor == 0)
        return;

    for (unsigned i = line->cursor - 1; i < line->length; i++)
        line->buffer[i] = line->buffer[i + 1];

    line->cursor--;
    line->length--;

    redraw_from_cursor(line);
}

static void delete_char(struct orbitline *line)
{
    if (!line || line->cursor >= line->length)
        return;

    for (unsigned i = line->cursor; i < line->length; i++)
        line->buffer[i] = line->buffer[i + 1];

    line->length--;
    redraw_from_cursor(line);
}

static void previous_word(struct orbitline *line)
{
    if (!line)
        return;

    while (line->cursor > 0 &&
           line->buffer[line->cursor - 1] == ' ')
        line->cursor--;

    while (line->cursor > 0 &&
           line->buffer[line->cursor - 1] != ' ')
        line->cursor--;

    redraw_from_cursor(line);
}

static void history_load(
    struct orbitline *line,
    int direction
) {
    if (!line || line->history_count == 0)
        return;

    line->history_position += direction;

    if (line->history_position < 0)
        line->history_position = 0;

    if (line->history_position >= (int)line->history_count)
        line->history_position = line->history_count - 1;

    unsigned position = (unsigned)line->history_position;

    unsigned i = 0;
    while (line->history[position][i] &&
           i < ORBITLINE_MAX_INPUT - 1) {
        line->buffer[i] = line->history[position][i];
        i++;
    }

    line->buffer[i] = 0;
    line->length = i;
    line->cursor = i;

    redraw_from_cursor(line);
}

void orbitline_init(
    struct orbitline *line,
    const struct orbitline_io *io
) {
    if (!line)
        return;

    line->length = 0;
    line->cursor = 0;
    line->history_count = 0;
    line->history_position = -1;
    line->buffer[0] = 0;
    line->io = io;
}

void orbitline_add_history(
    struct orbitline *line,
    const char *command
) {
    if (!line || !command || !command[0])
        return;

    unsigned i = 0;

    while (command[i] && i < ORBITLINE_MAX_INPUT - 1) {
        line->history[line->history_count][i] = command[i];
        i++;
    }

    line->history[line->history_count][i] = 0;

    if (line->history_count < ORBITLINE_HISTORY_SIZE - 1) {
        line->history_count++;
    } else {
        for (unsigned n = 1; n < ORBITLINE_HISTORY_SIZE; n++) {
            unsigned j = 0;

            while (line->history[n][j]) {
                line->history[n - 1][j] = line->history[n][j];
                j++;
            }

            line->history[n - 1][j] = 0;
        }
    }

    line->history_position = (int)line->history_count;
}

int orbitline_read(
    struct orbitline *line,
    char *output,
    unsigned output_size
) {
    if (!line || !line->io || !line->io->read_key ||
        !output || output_size == 0)
        return -1;

    line->length = 0;
    line->cursor = 0;
    line->history_position = (int)line->history_count;
    line->buffer[0] = 0;

    line->io->write_text("[UNIOS::ORBIT] pilot@ship:/ > ");

    for (;;) {
        int key = line->io->read_key();

        if (key == KEY_ENTER) {
            line->io->write_char('\n');

            unsigned i = 0;
            while (line->buffer[i] && i < output_size - 1) {
                output[i] = line->buffer[i];
                i++;
            }

            output[i] = 0;
            orbitline_add_history(line, output);
            return (int)i;
        }

        if (key == KEY_CTRL_C) {
            line->io->write_text("^C\n");
            output[0] = 0;
            return 0;
        }

        if (key == KEY_CTRL_L) {
            line->io->clear_screen();
            redraw_from_cursor(line);
            continue;
        }

        if (key == KEY_CTRL_U) {
            clear_line(line);
            continue;
        }

        if (key == KEY_CTRL_W) {
            while (line->cursor > 0 &&
                   line->buffer[line->cursor - 1] == ' ')
                backspace(line);

            while (line->cursor > 0 &&
                   line->buffer[line->cursor - 1] != ' ')
                backspace(line);

            continue;
        }

        if (key == KEY_BACKSPACE) {
            backspace(line);
            continue;
        }

        if (key == KEY_DELETE) {
            delete_char(line);
            continue;
        }

        if (key == KEY_LEFT && line->cursor > 0) {
            line->cursor--;
            line->io->move_cursor_left(1);
            continue;
        }

        if (key == KEY_RIGHT && line->cursor < line->length) {
            line->cursor++;
            line->io->move_cursor_right(1);
            continue;
        }

        if (key == KEY_HOME) {
            while (line->cursor > 0) {
                line->cursor--;
                line->io->move_cursor_left(1);
            }
            continue;
        }

        if (key == KEY_END) {
            while (line->cursor < line->length) {
                line->cursor++;
                line->io->move_cursor_right(1);
            }
            continue;
        }

        if (key == KEY_UP) {
            history_load(line, -1);
            continue;
        }

        if (key == KEY_DOWN) {
            history_load(line, 1);
            continue;
        }

        if (key == KEY_TAB) {
            /*
             * OrbitCLI completion will use the command registry and
             * filesystem names. For now, redraw the line.
             */
            line->io->write_text("\ncompletion query unavailable\n");
            redraw_from_cursor(line);
            continue;
        }

        if (key >= 32 && key <= 126)
            insert_char(line, (char)key);
    }
}
