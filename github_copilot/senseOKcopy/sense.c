/*
 * sense.c — Arrow-key input (Task A) + Sense HAT LED pixel (Task B)
 *
 * Build:  make
 * Test A: make test-keys   (no hardware needed)
 * Test B: make test-led    (requires Sense HAT on /dev/fb1)
 * Run:    ./sense          (full integration on Raspberry Pi with Sense HAT)
 *
 * Arrow keys move a single white pixel on the 8×8 LED matrix.
 * Press 'q' to quit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/mman.h>

/* ── Sense HAT framebuffer ─────────────────────────────────────────────── */
#define FB_DEV  "/dev/fb0" // Asked copilot "program runs, but no action at the sense hat", suggested changed from fb1 to fb0
#define WIDTH   8
#define HEIGHT  8
#define FB_SIZE (WIDTH * HEIGHT * sizeof(uint16_t))

/* ── Key codes ─────────────────────────────────────────────────────────── */
typedef enum {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_QUIT,
    KEY_OTHER
} Key;

/* ── Terminal raw mode ─────────────────────────────────────────────────── */
static struct termios orig_termios;

static void term_restore(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void term_raw_mode(void)
{
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    atexit(term_restore);

    raw = orig_termios;
    raw.c_lflag &= (tcflag_t)~(ECHO | ICANON | ISIG);
    raw.c_iflag &= (tcflag_t)~(IXON | ICRNL);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/* ── Arrow-key reader ──────────────────────────────────────────────────── */
static Key read_arrow(void)
{
    unsigned char buf[3] = {0};
    ssize_t n;

    n = read(STDIN_FILENO, &buf[0], 1);
    if (n <= 0)
        return KEY_OTHER;

    if (buf[0] == 'q' || buf[0] == 'Q')
        return KEY_QUIT;

    /* Arrow keys arrive as ESC [ A/B/C/D */
    if (buf[0] != 0x1B)
        return KEY_OTHER;

    n = read(STDIN_FILENO, &buf[1], 1);
    if (n <= 0 || buf[1] != '[')
        return KEY_OTHER;

    n = read(STDIN_FILENO, &buf[2], 1);
    if (n <= 0)
        return KEY_OTHER;

    switch (buf[2]) {
    case 'A': return KEY_UP;
    case 'B': return KEY_DOWN;
    case 'C': return KEY_RIGHT;
    case 'D': return KEY_LEFT;
    default:  return KEY_OTHER;
    }
}

/* ── Test A: keyboard only, no hardware ───────────────────────────────── */
static void run_test_keys(void)
{
    static const char *names[] = {"UP", "DOWN", "LEFT", "RIGHT", "QUIT", "OTHER"};

    printf("Arrow-key test — press arrow keys, 'q' to quit.\r\n");
    term_raw_mode();

    for (;;) {
        Key k = read_arrow();
        printf("Key: %s\r\n", names[k]);
        if (k == KEY_QUIT)
            break;
    }

    printf("Bye.\r\n");
}

/* ── RGB565 helper ─────────────────────────────────────────────────────── */
static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

/* ── Framebuffer helpers ───────────────────────────────────────────────── */
static uint16_t *fb_open(void)
{
    int fd = open(FB_DEV, O_RDWR);
    if (fd == -1) {
        perror("open " FB_DEV);
        exit(EXIT_FAILURE);
    }

    uint16_t *fb = mmap(NULL, FB_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);   /* fd can be closed after mmap */

    if (fb == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return fb;
}

static void fb_clear(uint16_t *fb)
{
    memset(fb, 0, FB_SIZE);
}

static void fb_set_pixel(uint16_t *fb, int x, int y, uint16_t color)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    fb[y * WIDTH + x] = color;
}

/* ── Test B: LED matrix, no keyboard ──────────────────────────────────── */
static void run_test_led(uint16_t *fb)
{
    /* Eight distinct colors, one per row */
    static const uint8_t colors[8][3] = {
        {255,   0,   0},  /* red    */
        {255, 128,   0},  /* orange */
        {255, 255,   0},  /* yellow */
        {  0, 255,   0},  /* green  */
        {  0,   0, 255},  /* blue   */
        { 75,   0, 130},  /* indigo */
        {148,   0, 211},  /* violet */
        {255, 255, 255},  /* white  */
    };

    printf("LED test — filling 8 rows with distinct colors for 3 s.\n");

    fb_clear(fb);
    for (int row = 0; row < HEIGHT; row++) {
        uint16_t c = rgb565(colors[row][0], colors[row][1], colors[row][2]);
        for (int col = 0; col < WIDTH; col++)
            fb_set_pixel(fb, col, row, c);
    }

    sleep(3);
    fb_clear(fb);
    printf("Done.\n");
}

/* ── Main integration loop ─────────────────────────────────────────────── */
static void run_main(uint16_t *fb)
{
    int px = WIDTH  / 2 - 1;   /* start near centre */
    int py = HEIGHT / 2 - 1;
    uint16_t white = 0xFFFF;

    printf("Use arrow keys to move the pixel. Press 'q' to quit.\r\n");
    term_raw_mode();

    fb_clear(fb);
    fb_set_pixel(fb, px, py, white);

    for (;;) {
        Key k = read_arrow();
        if (k == KEY_QUIT)
            break;

        switch (k) {
        case KEY_UP:    py = (py > 0)          ? py - 1 : py; break;
        case KEY_DOWN:  py = (py < HEIGHT - 1) ? py + 1 : py; break;
        case KEY_LEFT:  px = (px > 0)          ? px - 1 : px; break;
        case KEY_RIGHT: px = (px < WIDTH  - 1) ? px + 1 : px; break;
        default: break;
        }

        fb_clear(fb);
        fb_set_pixel(fb, px, py, white);
    }

    fb_clear(fb);
    printf("Bye.\r\n");
}

/* ── Entry point ───────────────────────────────────────────────────────── */
int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test-keys") == 0) {
        run_test_keys();
        return EXIT_SUCCESS;
    }

    uint16_t *fb = fb_open();

    if (argc == 2 && strcmp(argv[1], "--test-led") == 0) {
        run_test_led(fb);
    } else {
        run_main(fb);
    }

    munmap(fb, FB_SIZE);
    return EXIT_SUCCESS;
}
