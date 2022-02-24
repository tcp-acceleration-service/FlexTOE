/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/* Copyright (C) 2015-2018 Netronome Systems, Inc. */

/*
 * nfp_ioctl.h
 * ioctl definitions for the /dev/nfp-cpp-N devices
 * Author: Jason McMullan <jason.mcmullan@netronome.com>
 */
#ifndef NFP_IOCTL_H
#define NFP_IOCTL_H

#include <stdint.h>

/* NFP IOCTLs */
#define NFP_IOCTL	'n'

struct nfp_cpp_area_request {
	unsigned long   offset;		/* Offset for use by mmap() */
	unsigned int	cpp_id;		/* NFP_CPP_ID */
	unsigned long long cpp_addr;	/* Offset into area */
	unsigned long	size;		/* Requested area size */
};

/*
 * Request a NFP area, for later mapping via mmap()
 *
 * cmd is NFP_IOCTL_CPP_AREA_REQUEST,
 * Returns 0 on success and fills in 'offset', and an error on failure.
 */
#define NFP_IOCTL_CPP_AREA_REQUEST \
	_IOWR(NFP_IOCTL, 0x80, struct nfp_cpp_area_request)

/*
 * Release a NFP area, acquired earlier
 *
 * cmd is NFP_IOCTL_CPP_AREA_RELEASE, arg previously opened area
 * Match is done by all fields of the request, including offset
 */
#define NFP_IOCTL_CPP_AREA_RELEASE \
	_IOW(NFP_IOCTL, 0x81, struct nfp_cpp_area_request)

/*
 * Release a NFP area, acquired earlier (OBSOLETE API)
 *
 * cmd is NFP_IOCTL_CPP_AREA_RELEASE, arg offset from previous
 * call to NFP_IOCTL_CPP_AREA_REQUEST
 * Match is done by offset
 */
#define NFP_IOCTL_CPP_AREA_RELEASE_OBSOLETE \
	_IOW(NFP_IOCTL, 0x81, unsigned long)

#define NFP_IOCTL_CPP_EXPL_POST		0
#define NFP_IOCTL_CPP_EXPL1_BAR		1
#define NFP_IOCTL_CPP_EXPL2_BAR		2

/* When computing the CSR values, keep in mind:
 *  data_ref:      Only bit 2 is preserved
 *  signal_ref:    Only bit 0 is preserved
 */
struct nfp_cpp_explicit_request {
	unsigned long   csr[3];		/* CSR values */
	int		in, out;	/* in and out data length (# uint32_t) */
	uint32_t		data[32];	/* Data in/out */
	uint64_t		address;	/* CPP address */
};

/*
 * Perform a NFP explicit transaction
 *
 * cmd is NFP_IOCTL_CPP_EXPL_REQUEST,
 * Returns 0 on success, and an error on failure.
 */
#define NFP_IOCTL_CPP_EXPL_REQUEST \
	_IOW(NFP_IOCTL, 0x82, struct nfp_cpp_explicit_request)

/* This struct should only use uint32_t types where possible,
 * to reduce problems with packing differences on compilers.
 */
struct nfp_cpp_identification {
	uint32_t	size;		/* Size of this structure, in bytes */
	uint32_t	model;		/* NFP CPP model ID */
	uint32_t	interface;	/* NFP CPP interface ID */
	uint32_t	serial_lo;	/* Lower 32 of 48 bit serial number */
	uint32_t	serial_hi;	/* Upper 16 of 48 bit serial number */
};

/* Define a NFP event request */
struct nfp_cpp_event_request {
	int signal;
	int type;
	unsigned int match;
	unsigned int mask;
};

/**
 * Request a NFP event to be bound to a signal
 */
#define NFP_IOCTL_CPP_EVENT_ACQUIRE \
	_IOW(NFP_IOCTL, 0x83, struct nfp_cpp_event_request)

/**
 * Release a NFP event that was bound to a signal
 */
#define NFP_IOCTL_CPP_EVENT_RELEASE \
	_IOW(NFP_IOCTL, 0x84, struct nfp_cpp_event_request)

#define NFP_FIRMWARE_MAX  256           /* Maximum length of a firmware name */

/**
 * Request a firmware load
 */
#define NFP_IOCTL_FIRMWARE_LOAD \
	_IOW(NFP_IOCTL, 0x8d, char[NFP_FIRMWARE_MAX])

/**
 * Get the name of the last firmware load attempt
 */
#define NFP_IOCTL_FIRMWARE_LAST \
	_IOR(NFP_IOCTL, 0x8e, char[NFP_FIRMWARE_MAX])

/**
 * Request a NFP Identification structure
 *
 * cmd is NFP_IOCTL_CPP_IDENTIFICATION
 * The 'size' member of the passed-in struct nfp_cpp_identification
 * *must* be filled. Only members of the identification structure
 * up to 'size' will be filled in.
 *
 * Returns the total length supported on success, and an error on failure.
 */
#define NFP_IOCTL_CPP_IDENTIFICATION \
	_IOW(NFP_IOCTL, 0x8f, uint32_t)

#endif /* NFP_IOCTL_H */
