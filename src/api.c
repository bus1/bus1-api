/*
 * API Implementation
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <unistd.h>
#include "bus1-api.h"
#include "bus1-linux.h"

/*
 * Verify ABI Compatibility
 *
 * The following block verifies that the public ABI of `bus1-api.h` matches the
 * ABI provided by the kernel (which we have privately available as
 * `bus1-linux.h`). Since the kernel headers are not always distributed, the
 * bus1-api project does not depend on them in its public API. For performance
 * reasons, we still want the ABI to match, so we do not have to apply
 * conversions everywhere.
 */

#define B1API_ASSERT_ABI_SIZE(_a, _b) \
        static_assert(sizeof(_a) == sizeof(_b), "ABI Mismatch")

#define B1API_ASSERT_ABI_ALIGN(_a, _b) \
        static_assert(alignof(_a) == alignof(_b), "ABI Mismatch")

#define B1API_ASSERT_ABI_VALUE(_a, _b) \
        static_assert((_a) == (_b), "ABI Mismatch")

#define B1API_ASSERT_ABI_MEMBER(_a, _b, _member)                                \
        static_assert(offsetof(_a, _member) == offsetof(_b, _member),           \
                      "ABI Mismatch");                                          \
        B1API_ASSERT_ABI_SIZE(((_a *)(void *)0UL)->_member,                     \
                              ((_b *)(void *)0UL)->_member);                    \
        B1API_ASSERT_ABI_ALIGN(((_a *)(void *)0UL)->_member,                    \
                               ((_b *)(void *)0UL)->_member)

B1API_ASSERT_ABI_SIZE(BUS1API_INVALID, BUS1_INVALID);
B1API_ASSERT_ABI_ALIGN(BUS1API_INVALID, BUS1_INVALID);
B1API_ASSERT_ABI_VALUE(BUS1API_INVALID, BUS1_INVALID);

B1API_ASSERT_ABI_SIZE(Bus1ApiMessageType, enum bus1_message_type);
B1API_ASSERT_ABI_ALIGN(Bus1ApiMessageType, enum bus1_message_type);
B1API_ASSERT_ABI_VALUE((long)BUS1API_MESSAGE_TYPE_CUSTOM, BUS1_MESSAGE_TYPE_CUSTOM);
B1API_ASSERT_ABI_VALUE((long)BUS1API_MESSAGE_TYPE_RELEASE, BUS1_MESSAGE_TYPE_RELEASE);
B1API_ASSERT_ABI_VALUE((long)BUS1API_MESSAGE_TYPE_DESTRUCTION, BUS1_MESSAGE_TYPE_DESTRUCTION);
B1API_ASSERT_ABI_VALUE((long)_BUS1API_MESSAGE_TYPE_N, _BUS1_MESSAGE_TYPE_N);

B1API_ASSERT_ABI_SIZE(Bus1ApiTransfer, struct bus1_transfer);
B1API_ASSERT_ABI_ALIGN(Bus1ApiTransfer, struct bus1_transfer);
B1API_ASSERT_ABI_SIZE(Bus1ApiTransfer, struct bus1_transfer);
B1API_ASSERT_ABI_MEMBER(Bus1ApiTransfer, struct bus1_transfer, flags);
B1API_ASSERT_ABI_MEMBER(Bus1ApiTransfer, struct bus1_transfer, id);

B1API_ASSERT_ABI_SIZE(Bus1ApiMessage, struct bus1_message);
B1API_ASSERT_ABI_ALIGN(Bus1ApiMessage, struct bus1_message);
B1API_ASSERT_ABI_SIZE(Bus1ApiMessage, struct bus1_message);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, flags);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, type);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, n_transfers);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, ptr_transfers);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, n_data);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, n_data_vecs);
B1API_ASSERT_ABI_MEMBER(Bus1ApiMessage, struct bus1_message, ptr_data_vecs);

static int b1api_errno(void) {
        /*
         * This small helper replaces read-access to `errno`. It simply
         * verifies that `errno` is actually a positive integer (as required by
         * POSIX in case of an error). If not, it returns the catch-all error.
         * We mainly use this to make sure the compiler can rely on this and
         * optimize correctly.
         */
        return errno > 0 ? errno : ENOTRECOVERABLE;
}

static int b1api_ioctl(int fd, unsigned int cmd, void *arg) {
        int r;

        r = ioctl(fd, cmd, arg);
        if (r < 0)
                return -b1api_errno();

        return r;
}

int bus1api_open(int *fdp) {
        int fd;

        fd = open("/dev/bus1", O_RDWR | O_CLOEXEC | O_NONBLOCK | O_NOCTTY);
        if (fd < 0)
                return -b1api_errno();

        *fdp = fd;
        return 0;
}

int bus1api_close(int fd) {
        if (fd >= 0)
                close(fd);

        return -1;
}

int bus1api_pair(int fd1, int fd2, uint64_t flags, uint64_t (*ids)[2]) {
        struct bus1_cmd_pair cmd;
        int r;

        cmd = (struct bus1_cmd_pair){
                .flags = flags,
                .fd2 = fd2,
                .ids = { (*ids)[0], (*ids)[1] },
        };
        r = b1api_ioctl(fd1, BUS1_CMD_PAIR, &cmd);
        if (r < 0)
                return r;

        (*ids)[0] = cmd.ids[0];
        (*ids)[1] = cmd.ids[1];
        return r;
}

int bus1api_send(int fd, uint64_t flags, uint64_t n_dests, const uint64_t *dests, int *errors, const Bus1ApiMessage *m) {
        struct bus1_cmd_send cmd;

        cmd = (struct bus1_cmd_send){
                .flags = flags,
                .n_destinations = n_dests,
                .ptr_destinations = bus1api_from_ptr(dests),
                .ptr_errors = bus1api_from_ptr(errors),
                .ptr_message = bus1api_from_ptr(m),
        };
        return b1api_ioctl(fd, BUS1_CMD_SEND, &cmd);
}

int bus1api_recv(int fd, uint64_t flags, uint64_t *destp, Bus1ApiMessage *m) {
        struct bus1_cmd_recv cmd;
        int r;

        cmd = (struct bus1_cmd_recv){
                .flags = flags,
                .destination = *destp,
                .ptr_message = bus1api_from_ptr(m),
        };
        r = b1api_ioctl(fd, BUS1_CMD_RECV, &cmd);
        if (r < 0)
                return r;

        *destp = cmd.destination;
        return r;
}

int bus1api_destroy(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids) {
        struct bus1_cmd_destroy cmd;

        cmd = (struct bus1_cmd_destroy){
                .flags = flags,
                .n_ids = n_ids,
                .ptr_ids = bus1api_from_ptr(ids),
        };
        return b1api_ioctl(fd, BUS1_CMD_DESTROY, &cmd);
}

int bus1api_acquire(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids) {
        struct bus1_cmd_acquire cmd;

        cmd = (struct bus1_cmd_acquire){
                .flags = flags,
                .n_ids = n_ids,
                .ptr_ids = bus1api_from_ptr(ids),
        };
        return b1api_ioctl(fd, BUS1_CMD_ACQUIRE, &cmd);
}

int bus1api_release(int fd, uint64_t flags, uint64_t n_ids, const uint64_t *ids) {
        struct bus1_cmd_release cmd;

        cmd = (struct bus1_cmd_release){
                .flags = flags,
                .n_ids = n_ids,
                .ptr_ids = bus1api_from_ptr(ids),
        };
        return b1api_ioctl(fd, BUS1_CMD_RELEASE, &cmd);
}
