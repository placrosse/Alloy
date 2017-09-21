/* Alloy - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

/* Global includes */
#include <bmfs/bmfs.h>
#include <errno.h>
#include "libBareMetal.h"

/* Global macros */
#define BAREMETAL_PAGE_SIZE (2 * 1024 * 1024)

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
unsigned short running = 1;
unsigned char input[256];
int tokens;
uint64_t disk_offset = 0;
uint64_t disk_length = 1024 * 1024 * 6;
unsigned char disk_section[4096];

/* Strings */
const char alloy_version[] = "Alloy v0.0.1\n";
unsigned char cls_string[] = "cls";
unsigned char dir_string[] = "dir";
unsigned char ver_string[] = "ver";
unsigned char exit_string[] = "exit";
unsigned char help_string[] = "help";

/* Built-in functions*/
int str_cmp(unsigned char *str1, unsigned char *str2);
int str_parse(unsigned char str[]);
void str_chomp(unsigned char str[]);
int str_len(unsigned char str[]);
void list_files();
int load_app(const unsigned char app[]);
void mem_cpy(void *dest, const void *src, u64 n);

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
		if (str_cmp(input, exit_string) == 0)
			running = 0;
		else if (load_app(input) == 0)
			continue;
		else if (str_cmp(input, cls_string) == 0)
		{
			for (int i=0; i<25; i++)
				b_output("\n");
		}
		else if (str_cmp(input, ver_string) == 0)
			b_output(alloy_version);
		else if (str_cmp(input, dir_string) == 0)
			list_files();
		else if (str_cmp(input, help_string) == 0)
			b_output("Available commands: ver, cls, help, exit\n");
		else
		{
			if (tokens > 0)
				b_output("Unknown command\n");
		}
	}
}

/* string compare
 * returns 0 if equal, 1 if not equal
 */
int str_cmp(unsigned char *str1, unsigned char *str2)
{
	while ((*str1 == *str2) && *str1)
	{
		str1++;
		str2++;
	}

	if ((((int) (unsigned char) *str1) - ((int) (unsigned char) *str2)) == 0)
		return 0;
	else
		return 1;
}

/* string parse
 * Remove extra spaces and return number of words in string
 */
int str_parse(unsigned char str[])
{
	int i = 0;
	int c = 0;
	int x = 0;

	str_chomp(str); // remove leading and trailing spaces

	for(i=x=0; str[i]; ++i)
		if((str[i] != ' ') || (i>0 && (str[i-1] != ' ')))
			str[x++] = str[i];
	str[x] = '\0';

	x = 0;
	for (i=0; i<str_len(str); i++)
	{
		if (str[i] == ' ')
			x = 0;
		else
		{
			if (x == 0)
				c++;
			x = 1;
		}
	}

	return c;
}

/* string chomp
 * Remove leading and trailing spaces from a string
 */
void str_chomp(unsigned char str[])
{
	int i = 0, begin = 0;
	int end = str_len(str) - 1;

	while (str[begin] == ' ')
		begin++;

	while ((end >= begin) && (str[end] == ' '))
		end--;

	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0';
}

/* string length
 * returns the length of a string not including the null terminator
 */
int str_len(unsigned char str[])
{
	int count = 0;
	while (str[count] != '\0')
		count++;
	return count;
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

	err = bmfs_disk_read_dir(&disk, &dir);
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

int load_app(const unsigned char app[])
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

	err = bmfs_disk_read_dir(&disk, &dir);
	if (err != 0)
		return err;

	app_entry = bmfs_dir_find(&dir, (const char *)(app));
	if (app_entry == NULL)
		return -ENOENT;

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

	err = bmfs_disk_read(&disk, app_data, app_size, &app_size);
	if (err != 0)
		return err;

	b_smp_set(app_data, NULL /* data pointer */, 1 /* cpu index */);

	/* TODO : b_smp_wait(...); b_mem_release(app_data, app_pages); */

	return 0;
}

void mem_cpy(void *dest, const void *src, u64 n)
{
	u64 i;
	
	u8 *cdst = (u8 *)dest;
	const u8 *csrc = (const u8 *)src;
	
	for (i=0; i<n; i++)
		cdst[i] = csrc[i];
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

	mem_cpy(buf, &disk_section[disk_offset % 4096], bytes_read);

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

	mem_cpy(&disk_section[disk_offset % 4096], buf, buf_len);

	sections_write = b_disk_write(disk_section, disk_offset / 4096, 1, 0);

	bytes_write = sections_write * 4096;
	if (bytes_write > buf_len)
		bytes_write = buf_len;

	if (write_len != NULL)
		*write_len = bytes_write;

	return 0;
}

