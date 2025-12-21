#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>

#include <libopencm3/stm32/usart.h>

#include "pinout.h"

int _read(int fd, char *buffer, int size)
{
    (void)fd;

    for (int i = 0; i < size; i++) {
        *buffer++ = (char)usart_recv_blocking(CH340X_USART);
    }

    return size;
}

int _write(int fd, char *buffer, int size)
{
    (void)fd;

    for (int i = 0; i < size; i++) {
        usart_send_blocking(CH340X_USART, (uint16_t)*buffer++);
    }

    return size;
}

int _close(int fd)
{
    (void)fd;
    return -1;
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return 0;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}

#define MIN_STACK_SIZE (0x1000)

static uint8_t *heap_end = NULL;

void *_sbrk(ptrdiff_t incr)
{
    extern uint8_t end;
    extern uint8_t _stack;
    uint32_t stack_limit = (uint32_t)&_stack - MIN_STACK_SIZE;
    uint8_t *max_heap = (uint8_t *)stack_limit;
    uint8_t *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = &end;
    }

    if (heap_end + incr > max_heap) {
        errno = ENOMEM;
        return (void *)-1;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *)prev_heap_end;
}
