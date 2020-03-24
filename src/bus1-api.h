#pragma once

/*
 * Bindings to the Linux Bus1 Syscall API
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <sys/uio.h>

typedef struct  Bus1ApiMessage          Bus1ApiMessage;
typedef enum    Bus1ApiMessageType      Bus1ApiMessageType;
typedef struct  Bus1ApiTransfer         Bus1ApiTransfer;

#define BUS1API_INVALID ((uint64_t)-1)

enum Bus1ApiMessageType {
        BUS1API_MESSAGE_TYPE_CUSTOM,
        BUS1API_MESSAGE_TYPE_RELEASE,
        BUS1API_MESSAGE_TYPE_DESTRUCTION,
        _BUS1API_MESSAGE_TYPE_N,
};

struct Bus1ApiTransfer {
        uint64_t flags;
        uint64_t id;
} __attribute__((__aligned__(8)));

struct Bus1ApiMessage {
        uint64_t flags;
        uint64_t type;
        uint64_t n_transfers;
        uint64_t ptr_transfers;
        uint64_t n_data;
        uint64_t n_data_vecs;
        uint64_t ptr_data_vecs;
} __attribute__((__aligned__(8)));

/* syscalls */

int bus1api_open(int *fdp);
int bus1api_close(int fd);

int bus1api_pair(int fd1, int fd2, uint64_t flags, uint64_t (*idsp)[2]);
int bus1api_send(int fd, uint64_t flags, uint64_t n_dests, const uint64_t *dests, int *errors, const Bus1ApiMessage *m);
int bus1api_recv(int fd, uint64_t flags, uint64_t *destp, Bus1ApiMessage *m);
int bus1api_destroy(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids);
int bus1api_acquire(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids);
int bus1api_release(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids);

/* inline helpers */

static inline void *bus1api_to_ptr(const uint64_t ptr) {
        return (void *)(unsigned long)ptr;
}

static inline uint64_t bus1api_from_ptr(const void *ptr) {
        return (uint64_t)(unsigned long)ptr;
}

static inline void bus1api_closep(int *fd) {
        if (*fd >= 0)
                bus1api_close(*fd);
}

#ifdef __cplusplus
}
#endif
