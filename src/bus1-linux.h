/* SPDX-License-Identifier: LGPL-2.1+ */
#ifndef _UAPI_LINUX_BUS1_H
#define _UAPI_LINUX_BUS1_H

/**
 * DOC: Public Bus1 API
 *
 * Future Extensions:
 *  - promises
 *  - oneshot handles
 *  - FD-passing through FD-objects
 *  - managed IDs
 *
 * XXX
 */

#include <linux/ioctl.h>
#include <linux/types.h>

#define BUS1_IOCTL_MAGIC			0x97
#define BUS1_INVALID				((__u64)-1)
#define BUS1_MANAGED				(U64_C(0x1))

enum bus1_message_type {
	BUS1_MESSAGE_TYPE_CUSTOM,
	BUS1_MESSAGE_TYPE_RELEASE,
	BUS1_MESSAGE_TYPE_DESTRUCTION,
	_BUS1_MESSAGE_TYPE_N,
};

#define BUS1_TRANSFER_FLAG_HANDLE		(U64_C(1) <<  0)

struct bus1_transfer {
	__u64 flags;
	__u64 id;
} __attribute__((__aligned__(8)));

#define BUS1_MESSAGE_FLAG_MORE			(U64_C(1) <<  0)

struct bus1_message {
	__u64 flags;
	__u64 type;
	__u64 n_transfers;
	__u64 ptr_transfers;
	__u64 n_data;
	__u64 n_data_vecs;
	__u64 ptr_data_vecs;
} __attribute__((__aligned__(8)));

struct bus1_cmd_pair {
	__u64 flags;
	__u64 fd2;
	__u64 ids[2];
} __attribute__((__aligned__(8)));

struct bus1_cmd_send {
	__u64 flags;
	__u64 n_destinations;
	__u64 ptr_destinations;
	__u64 ptr_errors;
	__u64 ptr_message;
} __attribute__((__aligned__(8)));

#define BUS1_RECV_FLAG_TRUNCATE			(U64_C(1) <<  0)

struct bus1_cmd_recv {
	__u64 flags;
	__u64 destination;
	__u64 ptr_message;
} __attribute__((__aligned__(8)));

struct bus1_cmd_destroy {
	__u64 flags;
	__u64 n_ids;
	__u64 ptr_ids;
} __attribute__((__aligned__(8)));

struct bus1_cmd_acquire {
	__u64 flags;
	__u64 n_ids;
	__u64 ptr_ids;
} __attribute__((__aligned__(8)));

struct bus1_cmd_release {
	__u64 flags;
	__u64 n_ids;
	__u64 ptr_ids;
} __attribute__((__aligned__(8)));

enum bus1_cmd_type {
	BUS1_CMD_PAIR			= _IOWR(BUS1_IOCTL_MAGIC, 0x00,
					struct bus1_cmd_pair),
	BUS1_CMD_SEND			= _IOWR(BUS1_IOCTL_MAGIC, 0x01,
					struct bus1_cmd_send),
	BUS1_CMD_RECV			= _IOWR(BUS1_IOCTL_MAGIC, 0x02,
					struct bus1_cmd_recv),
	BUS1_CMD_DESTROY		= _IOWR(BUS1_IOCTL_MAGIC, 0x03,
					struct bus1_cmd_destroy),
	BUS1_CMD_ACQUIRE		= _IOWR(BUS1_IOCTL_MAGIC, 0x04,
					struct bus1_cmd_acquire),
	BUS1_CMD_RELEASE		= _IOWR(BUS1_IOCTL_MAGIC, 0x05,
					struct bus1_cmd_release),
};

#endif /* _UAPI_LINUX_BUS1_H */
