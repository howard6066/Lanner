/*
 * Copyright (C) 2005, 2012 IBM Corporation
 *
 * Authors:
 *	Kent Yoder <key@linux.vnet.ibm.com>
 *	Seiji Munetoh <munetoh@jp.ibm.com>
 *	Stefan Berger <stefanb@us.ibm.com>
 *	Reiner Sailer <sailer@watson.ibm.com>
 *	Kylene Hall <kjhall@us.ibm.com>
 *
 * Maintained by: <tpmdd-devel@lists.sourceforge.net>
 *
 * Access to the eventlog created by a system's firmware / BIOS
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 */

#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/security.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "tpm.h"
#include "tpm_eventlog.h"


static const char* tcpa_event_type_strings[] = {
	"PREBOOT",
	"POST CODE",
	"",
	"NO ACTION",
	"SEPARATOR",
	"ACTION",
	"EVENT TAG",
	"S-CRTM Contents",
	"S-CRTM Version",
	"CPU Microcode",
	"Platform Config Flags",
	"Table of Devices",
	"Compact Hash",
	"IPL",
	"IPL Partition Data",
	"Non-Host Code",
	"Non-Host Config",
	"Non-Host Info"
};

static const char* tcpa_pc_event_id_strings[] = {
	"",
	"SMBIOS",
	"BIS Certificate",
	"POST BIOS ",
	"ESCD ",
	"CMOS",
	"NVRAM",
	"Option ROM",
	"Option ROM config",
	"",
	"Option ROM microcode ",
	"S-CRTM Version",
	"S-CRTM Contents ",
	"POST Contents ",
	"Table of Devices",
};

/* returns pointer to start of pos. entry of tcg log */
static void *tpm_bios_measurements_start(struct seq_file *m, loff_t *pos)
{
	loff_t i;
	struct tpm_bios_log *log = m->private;
	void *addr = log->bios_event_log;
	void *limit = log->bios_event_log_end;
	struct tcpa_event *event;
	u32 converted_event_size;
	u32 converted_event_type;


	/* read over *pos measurements */
	for (i = 0; i < *pos; i++) {
		event = addr;

		converted_event_size =
		    do_endian_conversion(event->event_size);
		converted_event_type =
		    do_endian_conversion(event->event_type);

		if ((addr + sizeof(struct tcpa_event)) < limit) {
			if ((converted_event_type == 0) &&
			    (converted_event_size == 0))
				return null;
			addr += (sizeof(struct tcpa_event) +
				 converted_event_size);
		}
	}

	/* now check if current entry is valid */
	if ((addr + sizeof(struct tcpa_event)) >= limit)
		return null;

	event = addr;

	converted_event_size = do_endian_conversion(event->event_size);
	converted_event_type = do_endian_conversion(event->event_type);

	if (((converted_event_type == 0) && (converted_event_size == 0))
	    || ((addr + sizeof(struct tcpa_event) + converted_event_size)
		>= limit))
		return null;

	return addr;
}

static void *tpm_bios_measurements_next(struct seq_file *m, void *v,
					loff_t *pos)
{
	struct tcpa_event *event = v;
	struct tpm_bios_log *log = m->private;
	void *limit = log->bios_event_log_end;
	u32 converted_event_size;
	u32 converted_event_type;

	converted_event_size = do_endian_conversion(event->event_size);

	v += sizeof(struct tcpa_event) + converted_event_size;

	/* now check if current entry is valid */
	if ((v + sizeof(struct tcpa_event)) >= limit)
		return NULL;

	event = v;

	converted_event_size = do_endian_conversion(event->event_size);
	converted_event_type = do_endian_conversion(event->event_type);

	if (((converted_event_type == 0) && (converted_event_size == 0)) ||
	    ((v + sizeof(struct tcpa_event) + converted_event_size) >= limit))
		return NULL;

	(*pos)++;
	return v;
}

static void tpm_bios_measurements_stop(struct seq_file *m, void *v)
{
}

static int get_event_name(char *dest, struct tcpa_event *event,
			unsigned char * event_entry)
{
	const char *name = "";
	/* 41 so there is room for 40 data and 1 nul */
	char data[41] = "";
	int i, n_len = 0, d_len = 0;
	struct tcpa_pc_event *pc_event;

	switch (do_endian_conversion(event->event_type)) {
	case PREBOOT:
	case POST_CODE:
	case UNUSED:
	case NO_ACTION:
	case SCRTM_CONTENTS:
	case SCRTM_VERSION:
	case CPU_MICROCODE:
	case PLATFORM_CONFIG_FLAGS:
	case TABLE_OF_DEVICES:
	case COMPACT_HASH:
	case IPL:
	case IPL_PARTITION_DATA:
	case NONHOST_CODE:
	case NONHOST_CONFIG:
	case NONHOST_INFO:
		name = tcpa_event_type_strings[do_endian_conversion
						(event->event_type)];
		n_len = strlen(name);
		break;
	case SEPARATOR:
	case ACTION:
		if (MAX_TEXT_EVENT >
		    do_endian_conversion(event->event_size)) {
			name = event_entry;
			n_len = do_endian_conversion(event->event_size);
		}
		break;
	case EVENT_TAG:
		pc_event = (struct tcpa_pc_event *)event_entry;

		/* ToDo Row data -> Base64 */

		switch (do_endian_conversion(pc_event->event_id)) {
		case SMBIOS:
		case BIS_CERT:
		case CMOS:
		case NVRAM:
		case OPTION_ROM_EXEC:
		case OPTION_ROM_CONFIG:
		case S_CRTM_VERSION:
			name = tcpa_pc_event_id_strings[do_endian_conversion
							(pc_event->event_id)];
			n_len = strlen(name);
			break;
		/* hash data */
		case POST_BIOS_ROM:
		case ESCD:
		case OPTION_ROM_MICROCODE:
		case S_CRTM_CONTENTS:
		case POST_CONTENTS:
			name = tcpa_pc_event_id_strings[do_endian_conversion
							(pc_event->event_id)];
			n_len = strlen(name);
			for (i = 0; i < 20; i++)
				d_len += sprintf(&data[2*i], "%02x",
						pc_event->event_data[i]);
			break;
		default:
			break;
		}
	default:
		break;
	}

	return snprintf(dest, MAX_TEXT_EVENT, "[%.*s%.*s]",
			n_len, name, d_len, data);

}

static int tpm_binary_bios_measurements_show(struct seq_file *m, void *v)
{
	struct tcpa_event *event = v;
	struct tcpa_event temp_event;
	char *temp_ptr;
	int i;

	memcpy(&temp_event, event, sizeof(struct tcpa_event));

	/* convert raw integers for endianness */
	temp_event.pcr_index = do_endian_conversion(event->pcr_index);
	temp_event.event_type = do_endian_conversion(event->event_type);
	temp_event.event_size = do_endian_conversion(event->event_size);

	temp_ptr = (char *) &temp_event;

	for (i = 0; i < (sizeof(struct tcpa_event) - 1) ; i++)
		seq_putc(m, temp_ptr[i]);

	temp_ptr = (char *) v;

	for (i = (sizeof(struct tcpa_event) - 1);
	     i < (sizeof(struct tcpa_event) + temp_event.event_size); i++)
		seq_putc(m, temp_ptr[i]);

	return 0;

}

static int tpm_bios_measurements_release(struct inode *inode,
					 struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct tpm_bios_log *log = seq->private;

	if (log) {
		kfree(log->bios_event_log);
		kfree(log);
	}

	return seq_release(inode, file);
}

static int tpm_ascii_bios_measurements_show(struct seq_file *m, void *v)
{
	int len = 0;
	char *eventname;
	struct tcpa_event *event = v;
	unsigned char *event_entry =
	    (unsigned char *)(v + sizeof(struct tcpa_event));

	eventname = kmalloc(MAX_TEXT_EVENT, GFP_KERNEL);
	if (!eventname) {
		printk(KERN_ERR "%s: ERROR - No Memory for event name\n ",
		       __func__);
		return -EFAULT;
	}

	/* 1st: PCR */
	seq_printf(m, "%2d ", do_endian_conversion(event->pcr_index));

	/* 2nd: SHA1 */
	seq_printf(m, "%20phN", event->pcr_value);

	/* 3rd: event type identifier */
	seq_printf(m, " %02x", do_endian_conversion(event->event_type));

	len += get_event_name(eventname, event, event_entry);

	/* 4th: eventname <= max + \'0' delimiter */
	seq_printf(m, " %s\n", eventname);

	kfree(eventname);
	return 0;
}

static const struct seq_operations tpm_ascii_b_measurments_seqops = {
	.start = tpm_bios_measurements_start,
	.next = tpm_bios_measurements_next,
	.stop = tpm_bios_measurements_stop,
	.show = tpm_ascii_bios_measurements_show,
};

static const struct seq_operations tpm_binary_b_measurments_seqops = {
	.start = tpm_bios_measurements_start,
	.next = tpm_bios_measurements_next,
	.stop = tpm_bios_measurements_stop,
	.show = tpm_binary_bios_measurements_show,
};

static int tpm_ascii_bios_measurements_open(struct inode *inode,
					    struct file *file)
{
	int err;
	struct tpm_bios_log *log;
	struct seq_file *seq;

	log = kzalloc(sizeof(struct tpm_bios_log), GFP_KERNEL);
	if (!log)
		return -ENOMEM;

	if ((err = read_log(log)))
		goto out_free;

	/* now register seq file */
	err = seq_open(file, &tpm_ascii_b_measurments_seqops);
	if (!err) {
		seq = file->private_data;
		seq->private = log;
	} else {
		goto out_free;
	}

out:
	return err;
out_free:
	kfree(log->bios_event_log);
	kfree(log);
	goto out;
}

static const struct file_operations tpm_ascii_bios_measurements_ops = {
	.open = tpm_ascii_bios_measurements_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = tpm_bios_measurements_release,
};

static int tpm_binary_bios_measurements_open(struct inode *inode,
					     struct file *file)
{
	int err;
	struct tpm_bios_log *log;
	struct seq_file *seq;

	log = kzalloc(sizeof(struct tpm_bios_log), GFP_KERNEL);
	if (!log)
		return -ENOMEM;

	if ((err = read_log(log)))
		goto out_free;

	/* now register seq file */
	err = seq_open(file, &tpm_binary_b_measurments_seqops);
	if (!err) {
		seq = file->private_data;
		seq->private = log;
	} else {
		goto out_free;
	}

out:
	return err;
out_free:
	kfree(log->bios_event_log);
	kfree(log);
	goto out;
}

static const struct file_operations tpm_binary_bios_measurements_ops = {
	.open = tpm_binary_bios_measurements_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = tpm_bios_measurements_release,
};

static int is_bad(void *p)
{
	if (!p)
		return 1;
	if (IS_ERR(p) && (PTR_ERR(p) != -ENODEV))
		return 1;
	return 0;
}

struct dentry **tpm_bios_log_setup(char *name)
{
	struct dentry **ret = NULL, *tpm_dir, *bin_file, *ascii_file;

	tpm_dir = securityfs_create_dir(name, NULL);
	if (is_bad(tpm_dir))
		goto out;

	bin_file =
	    securityfs_create_file("binary_bios_measurements",
				   S_IRUSR | S_IRGRP, tpm_dir, NULL,
				   &tpm_binary_bios_measurements_ops);
	if (is_bad(bin_file))
		goto out_tpm;

	ascii_file =
	    securityfs_create_file("ascii_bios_measurements",
				   S_IRUSR | S_IRGRP, tpm_dir, NULL,
				   &tpm_ascii_bios_measurements_ops);
	if (is_bad(ascii_file))
		goto out_bin;

	ret = kmalloc(3 * sizeof(struct dentry *), GFP_KERNEL);
	if (!ret)
		goto out_ascii;

	ret[0] = ascii_file;
	ret[1] = bin_file;
	ret[2] = tpm_dir;

	return ret;

out_ascii:
	securityfs_remove(ascii_file);
out_bin:
	securityfs_remove(bin_file);
out_tpm:
	securityfs_remove(tpm_dir);
out:
	return NULL;
}

void tpm_bios_log_teardown(struct dentry **lst)
{
	int i;

	for (i = 0; i < 3; i++)
		securityfs_remove(lst[i]);
}
EXPORT_SYMBOL_GPL(tpm_bios_log_teardown);
