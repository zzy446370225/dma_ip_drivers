/*
 * Copyright(c) 2019 Xilinx, Inc. All rights reserved.
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */

#include "qdma_access_common.h"
#include "qdma_reg.h"
#include "qdma_reg_dump.h"


void qdma_write_csr_values(void *dev_hndl, uint32_t reg_offst,
		uint32_t idx, uint32_t cnt, const uint32_t *values)
{
	uint32_t index, reg_addr;

	for (index = idx; index < (idx + cnt); index++) {
		reg_addr = reg_offst + (index * sizeof(uint32_t));
		qdma_reg_write(dev_hndl, reg_addr, values[index - idx]);
	}
}

void qdma_read_csr_values(void *dev_hndl, uint32_t reg_offst,
		uint32_t idx, uint32_t cnt, uint32_t *values)
{
	uint32_t index, reg_addr;

	reg_addr = reg_offst + (idx * sizeof(uint32_t));
	for (index = 0; index < cnt; index++) {
		values[index] = qdma_reg_read(dev_hndl, reg_addr +
					      (index * sizeof(uint32_t)));
	}
}

/*
 * dump_reg() - Helper function to dump register value into string
 *
 * return len - length of the string copied into buffer
 */
int dump_reg(char *buf, int buf_sz, unsigned int raddr,
		const char *rname, unsigned int rval)
{
	/* length of the line should be minimum 80 chars.
	 * If below print pattern is changed, check for
	 * new buffer size requirement
	 */
	if (buf_sz < DEBGFS_LINE_SZ) {
		qdma_log_error("%s: buf_sz(%d) < expected(%d): err: %d\n",
						__func__,
						buf_sz, DEBGFS_LINE_SZ,
					   -QDMA_ERR_INV_PARAM);
		return -QDMA_ERR_INV_PARAM;
	}

	return QDMA_SNPRINTF(buf, DEBGFS_LINE_SZ,
			"[%#7x] %-47s %#-10x %u\n",
			raddr, rname, rval, rval);

}

/*
 * dump_context() - Helper function to dump queue context into string
 *
 * return len - length of the string copied into buffer
 */
int dump_context(char *buf, int buf_sz,
	char pfetch_valid, char cmpt_valid)
{
	int i = 0;
	int n;
	int len = 0;
	char banner[DEBGFS_LINE_SZ];

	for (i = 0; i < DEBGFS_LINE_SZ - 5; i++)
		QDMA_SNPRINTF(banner + i, DEBGFS_LINE_SZ - 5, "-");

	/* SW context dump */
	n = sizeof(sw_ctxt_entries) / sizeof((sw_ctxt_entries)[0]);
	for (i = 0; i < n; i++) {
		if ((len >= buf_sz) || ((len + DEBGFS_LINE_SZ) >= buf_sz))
			goto INSUF_BUF_EXIT;

		if (i == 0) {
			if ((len + (3 * DEBGFS_LINE_SZ)) >= buf_sz)
				goto INSUF_BUF_EXIT;

			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s", banner);
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%40s", "SW Context");
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s\n", banner);
		}

		len += QDMA_SNPRINTF(buf + len, DEBGFS_LINE_SZ,
			"%-47s %#-10x %u\n",
			sw_ctxt_entries[i].name,
			sw_ctxt_entries[i].value,
			sw_ctxt_entries[i].value);
	}

	/* HW context dump */
	n = sizeof(hw_ctxt_entries) / sizeof((hw_ctxt_entries)[0]);
	for (i = 0; i < n; i++) {
		if ((len >= buf_sz) || ((len + DEBGFS_LINE_SZ) >= buf_sz))
			goto INSUF_BUF_EXIT;

		if (i == 0) {
			if ((len + (3 * DEBGFS_LINE_SZ)) >= buf_sz)
				goto INSUF_BUF_EXIT;

			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s", banner);
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%40s", "HW Context");
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s\n", banner);
		}

		len += QDMA_SNPRINTF(buf + len, DEBGFS_LINE_SZ,
			"%-47s %#-10x %u\n",
			hw_ctxt_entries[i].name, hw_ctxt_entries[i].value,
			hw_ctxt_entries[i].value);
	}

	/* Credit context dump */
	n = sizeof(credit_ctxt_entries) / sizeof((credit_ctxt_entries)[0]);
	for (i = 0; i < n; i++) {
		if ((len >= buf_sz) || ((len + DEBGFS_LINE_SZ) >= buf_sz))
			goto INSUF_BUF_EXIT;

		if (i == 0) {
			if ((len + (3 * DEBGFS_LINE_SZ)) >= buf_sz)
				goto INSUF_BUF_EXIT;

			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s", banner);
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%40s", "Credit Context");
			len += QDMA_SNPRINTF(buf + len,
				DEBGFS_LINE_SZ, "\n%s\n", banner);
		}

		len += QDMA_SNPRINTF(buf + len, DEBGFS_LINE_SZ,
			"%-47s %#-10x %u\n",
			credit_ctxt_entries[i].name,
			credit_ctxt_entries[i].value,
			credit_ctxt_entries[i].value);
	}

	if (cmpt_valid) {
		/* Completion context dump */
		n = sizeof(cmpt_ctxt_entries) / sizeof((cmpt_ctxt_entries)[0]);
		for (i = 0; i < n; i++) {
			if ((len >= buf_sz) ||
				((len + DEBGFS_LINE_SZ) >= buf_sz))
				goto INSUF_BUF_EXIT;

			if (i == 0) {
				if ((len + (3 * DEBGFS_LINE_SZ)) >= buf_sz)
					goto INSUF_BUF_EXIT;

				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%s", banner);
				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%40s",
					"Completion Context");
				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%s\n", banner);

			}

			len += QDMA_SNPRINTF(buf + len, DEBGFS_LINE_SZ,
				"%-47s %#-10x %u\n",
				cmpt_ctxt_entries[i].name,
				cmpt_ctxt_entries[i].value,
				cmpt_ctxt_entries[i].value);
		}
	}
	if (pfetch_valid) {
		/* Prefetch context dump */
		n = sizeof(c2h_pftch_ctxt_entries) /
			sizeof(c2h_pftch_ctxt_entries[0]);
		for (i = 0; i < n; i++) {
			if ((len >= buf_sz) ||
				((len + DEBGFS_LINE_SZ) >= buf_sz))
				goto INSUF_BUF_EXIT;

			if (i == 0) {
				if ((len + (3 * DEBGFS_LINE_SZ)) >= buf_sz)
					goto INSUF_BUF_EXIT;

				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%s", banner);
				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%40s",
					"Prefetch Context");
				len += QDMA_SNPRINTF(buf + len,
					DEBGFS_LINE_SZ, "\n%s\n", banner);

			}

			len += QDMA_SNPRINTF(buf + len, DEBGFS_LINE_SZ,
				"%-47s %#-10x %u\n",
				c2h_pftch_ctxt_entries[i].name,
				c2h_pftch_ctxt_entries[i].value,
				c2h_pftch_ctxt_entries[i].value);
		}
	}

	return len;

INSUF_BUF_EXIT:
	len += QDMA_SNPRINTF((buf + len - DEBGFS_LINE_SZ), DEBGFS_LINE_SZ,
		"\n\nInsufficient buffer size, partial context dump\n");

	qdma_log_error("%s: Insufficient buffer size, err:%d\n",
		__func__, -QDMA_ERR_NO_MEM);

	return -QDMA_ERR_NO_MEM;
}

void qdma_memset(void *to, uint8_t val, uint32_t size)
{
	uint32_t i;
	uint8_t *_to = (uint8_t *)to;

	for (i = 0; i < size; i++)
		_to[i] = val;
}
