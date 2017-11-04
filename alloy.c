/* Alloy - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

/* Global includes */
#include <baremetal/syscalls.h>
#include <bmfs/bmfs.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* Global macros */
#ifndef BAREMETAL_PAGE_SIZE
#define BAREMETAL_PAGE_SIZE (2 * 1024 * 1024)
#endif

#ifndef ARGC_MAX
#define ARGC_MAX 3
#endif

/* Global typedefs */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

/* Global variables */
struct app_info {
	int argc;
	char *argv[ARGC_MAX + 1];
	int (*main)(int argc, char **arv);
};
/* there's 64 bytes to mess with at
 * this address. make sure app_info
 * does not exceed this limit. */
struct app_info *app_info = (struct app_info *) 0x110192;
unsigned short running = 1;
char input[256];
int tokens;
uint64_t disk_offset = 0;
uint64_t disk_length = 1024 * 1024 * 6;
unsigned char disk_section[4096];

/* Strings */
const char alloy_version[] = "Alloy v0.0.1\n";
char cls_string[] = "cls";
char dir_string[] = "dir";
char ver_string[] = "ver";
char exit_string[] = "exit";
char help_string[] = "help";

/* Built-in functions*/
size_t str_parse(char str[]);
void str_chomp(char str[]);
void list_files();
int load_app(char app[], int tokens);

/* disk functions */
int alloy_seek(void *disk_data, int64_t offset, int whence);
int alloy_tell(void *disk_data, int64_t *offset);
int alloy_read(void *disk_data, void *buf, uint64_t buf_len, uint64_t *read_len);
int alloy_write(void *disk_data, const void *buf, uint64_t buf_len, uint64_t *write_len);

/* Program code */
int main()
{
	for (int i=0; i<25; i++)
		b_output("\n");
	b_output(alloy_version);
	while (running == 1)
	{
		b_output("> ");
		b_input(input, 255);
		tokens = str_parse(input);
		if (strcmp(input, exit_string) == 0)
			running = 0;
		else if (load_app(input, tokens) == 0)
			continue;
		else if (strcmp(input, cls_string) == 0)
		{
			for (int i=0; i<25; i++)
				b_output("\n");
		}
		else if (strcmp(input, ver_string) == 0)
			b_output(alloy_version);
		else if (strcmp(input, dir_string) == 0)
			list_files();
		else if (strcmp(input, help_string) == 0)
			b_output("Available commands: ver, cls, help, exit\n");
		else
		{
			if (tokens > 0)
				puts("Unknown command\n");
		}
	}
}

/* string parse
 * Remove extra spaces and return number of words in string
 */
size_t str_parse(char str[])
{
	str_chomp(str); // remove leading and trailing spaces

	size_t len = strlen(str);
	size_t x = 0;
	int tokens = 0;

	for (size_t i = 0; i < len; i++)
	{
		if (str[i] == ' ')
		{
			str[i] = 0;
			x = 0;
		}
		else
		{
			if (x == 0)
				tokens++;
			x = 1;
		}
	}

	return tokens;
}

/* string chomp
 * Remove leading and trailing spaces from a string
 */
void str_chomp(char str[])
{
	int i = 0, begin = 0;
	int end = strlen(str) - 1;

	while (str[begin] == ' ')
		begin++;

	while ((end >= begin) && (str[end] == ' '))
		end--;

	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0';
}

void list_files()
{
	int err;
	uint64_t i;
	struct BMFSEntry *entry;
	struct BMFSDir dir;
	struct BMFSDisk disk;

	disk.disk = NULL;
	disk.seek = alloy_seek;
	disk.tell = alloy_tell;
	disk.read = alloy_read;
	disk.write = alloy_write;

	err = bmfs_disk_read_root_dir(&disk, &dir);
	if (err != 0)
	{
		b_output("failed to read directory");
		return;
	}

	for (i = 0; i < 64; i++)
	{
		entry = &dir.Entries[i];
		if (bmfs_entry_is_empty(entry))
			continue;
		else if (bmfs_entry_is_terminator(entry))
			continue;

		b_output(entry->FileName);
		b_output("\n");
	}
}

static void app_start(void)
{
	app_info->main(app_info->argc, app_info->argv);
}

int load_app(char app[], int tokens)
{
	int err;
	uint64_t app_offset;
	struct BMFSEntry *app_entry;
	struct BMFSDir dir;
	struct BMFSDisk disk;
	void *app_data = NULL;
	uint64_t app_size = 0;
	uint64_t app_pages = 0;

	disk.disk = NULL;
	disk.tell = alloy_tell;
	disk.seek = alloy_seek;
	disk.read = alloy_read;
	disk.write = alloy_write;

	err = bmfs_disk_read_root_dir(&disk, &dir);
	if (err != 0)
		return err;

	app_entry = bmfs_dir_find(&dir, (const char *)(app));
	if (app_entry == NULL)
	{
		return -ENOENT;
	}

	err = bmfs_entry_get_offset(app_entry, &app_offset);
	if (err != 0)
		return err;

	err = bmfs_disk_seek(&disk, app_offset, SEEK_SET);
	if (err != 0)
		return err;

	app_size = app_entry->FileSize;

	app_pages = app_size / BAREMETAL_PAGE_SIZE;
	/* because of integer truncation when app size % page size != 0,
	 * round up one page to make sure enough memory is available */
	app_pages++;

	b_mem_allocate(&app_data, app_pages);

	unsigned char *app_data8 = (unsigned char *) app_data;

	uint64_t total_read_size = 0;
	while (total_read_size < app_size) {
		uint64_t read_size = 0;
		err = bmfs_disk_read(&disk, &app_data8[total_read_size], app_size - total_read_size, &read_size);
		if (err != 0)
			return err;
		total_read_size += read_size;
	}

	app_info->main = app_data;
	app_info->argc = 0;
	app_info->argv[0] = NULL;

	for (int i = 0; i < ARGC_MAX + 1; i++)
		app_info->argv[i] = NULL;

	int j = 0;
	for (int i = 0; (i < tokens) && (i < ARGC_MAX); i++) {

		app_info->argv[i] = &app[j];
		app_info->argc++;

		while (app[j] != 0)
			j++;

		j++;
	}

	app_start();

	return 0;
}

int alloy_seek(void *disk_data, int64_t offset, int whence)
{
	(void) disk_data;

	if (whence == SEEK_SET)
		disk_offset = offset;
	else if (whence == SEEK_END)
		disk_offset = disk_length - offset;
	else if (whence == SEEK_CUR)
		disk_offset += offset;
	else
		return -EINVAL;

	return 0;
}

int alloy_tell(void *disk_data, int64_t *offset)
{
	(void) disk_data;
	*offset = disk_offset;
	return 0;
}

int alloy_read(void *disk_data, void *buf, uint64_t buf_len, uint64_t *read_len)
{
	(void) disk_data;

	uint64_t sections_read;
	uint64_t bytes_read;

	if (buf_len > 4096)
		buf_len = 4096;

	sections_read = b_disk_read(disk_section, disk_offset / 4096, 1, 0);

	bytes_read = sections_read * 4096;
	if (bytes_read > buf_len)
		bytes_read = buf_len;

	if (read_len != NULL)
		*read_len = bytes_read;

	memcpy(buf, &disk_section[disk_offset % 4096], bytes_read);

	disk_offset += bytes_read;

	return 0;
}

int alloy_write(void *disk_data, const void *buf, uint64_t buf_len, uint64_t *write_len)
{
	(void) disk_data;

	uint64_t sections_read;
	uint64_t sections_write;
	uint64_t bytes_write;

	sections_read = b_disk_read(disk_section, disk_offset / 4096, 1, 0);
	if (sections_read == 0)
		return -EIO;

	memcpy(&disk_section[disk_offset % 4096], buf, buf_len);

	sections_write = b_disk_write(disk_section, disk_offset / 4096, 1, 0);

	bytes_write = sections_write * 4096;
	if (bytes_write > buf_len)
		bytes_write = buf_len;

	if (write_len != NULL)
		*write_len = bytes_write;

	return 0;
}

