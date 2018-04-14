/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/color.h>
#include <alloy/shell.h>
#include <alloy/input.h>
#include <alloy/types.h>

#if defined ALLOY_PLATFORM_linux
#include <alloy/xterm.h>
#elif defined ALLOY_PLATFORM_baremetal
#include <alloy/vesaterm.h>
#endif

/* Strings */
const char alloy_version[] = "Alloy v0.0.2\n";
char cls_string[] = "cls";
char dir_string[] = "dir";
char ver_string[] = "ver";
char exit_string[] = "exit";
char help_string[] = "help";

/* Built-in functions*/
alloy_size str_parse(char str[]);
alloy_size alloy_strlen(const char *str);
void str_chomp(char str[]);
void list_files(struct AlloyShell *shell);
int load_app(char app[], int tokens);
void printf_d(struct AlloyShell *shell, int val);

/* syscall hook */
int alloy_output(const char *message, unsigned long long int message_length);

char input_buf[512];

static alloy_bool is_cmd(const char *arg, const char *cmd)
{
	while (!!(*arg) && !!(*cmd))
	{
		if (*arg != *cmd)
			break;

		arg++;
		cmd++;
	}

	if ((*arg) != (*cmd))
		return ALLOY_FALSE;

	return ALLOY_TRUE;
}

/* Program code */
int main(void)
{
	/* setup the input structure */

	struct AlloyInput input;

	alloy_input_init(&input);

	input.buf = input_buf;
	input.buf_res = sizeof(input_buf);

	/* setup the shell */

	struct AlloyShell shell;

	alloy_shell_init(&shell);

	alloy_shell_set_input(&shell, &input);

#if defined ALLOY_PLATFORM_linux
	alloy_shell_set_term(&shell, &alloy_xterm);
#elif defined ALLOY_PLATFORM_baremetal
	alloy_shell_set_term(&shell, &alloy_vesaterm);
#endif

	for (;;)
	{
		if (shell.QuitFlag)
			break;

		alloy_input_clear(shell.Input);

		alloy_shell_write_asciiz(&shell, "> ");

		alloy_shell_get_line(&shell);

		alloy_shell_write_asciiz(&shell, "\n");

		int tokens = str_parse(input_buf);

		if (is_cmd(input_buf, exit_string))
			break;
		else if (load_app(input_buf, tokens))
			continue;
		else if (is_cmd(input_buf, cls_string))
			alloy_shell_clear(&shell);
		else if (is_cmd(input_buf, ver_string))
			alloy_shell_write_asciiz(&shell, alloy_version);
		else if (is_cmd(input_buf, dir_string))
			list_files(&shell);
		else if (is_cmd(input_buf, help_string))
		{
			alloy_shell_write_asciiz(&shell, "Available commands:\n");
			alloy_shell_write_asciiz(&shell, "\tcls  : Clear the screen.\n");
			alloy_shell_write_asciiz(&shell, "\tdir  : List the directory entries.\n");
			alloy_shell_write_asciiz(&shell, "\texit : Exit the shell.\n");
			alloy_shell_write_asciiz(&shell, "\thelp : Print this help message.\n");
			alloy_shell_write_asciiz(&shell, "\tver  : Print the current version of Alloy.\n");
		}
		else
		{
			if (tokens > 0)
			{
				alloy_shell_set_foreground(&shell, &alloy_red);
				alloy_shell_write_asciiz(&shell, "Unknown command: ");
				alloy_shell_set_foreground(&shell, &alloy_white);
				alloy_shell_write_asciiz(&shell, shell.Input->buf);
				alloy_shell_write_asciiz(&shell, "\n");
			}
		}
	}

	alloy_shell_done(&shell);

	return 0;
}

/* string parse
 * Remove extra spaces and return number of words in string
 */
alloy_size str_parse(char str[])
{
	str_chomp(str); // remove leading and trailing spaces

	alloy_size x = 0;
	int tokens = 0;

	for (alloy_size i = 0; str[i] != 0; i++)
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

void list_files(struct AlloyShell *shell)
{
	alloy_shell_write_asciiz(shell, "Not implemented.\n");
}

int load_app(char app[], int tokens)
{
	(void) app;
	(void) tokens;
	return 0;
}

/* string chomp
 * Remove leading and trailing spaces from a string
 */
void str_chomp(char str[])
{
	int i = 0, begin = 0;
	int end = alloy_strlen(str) - 1;

	while (str[begin] == ' ')
		begin++;

	while ((end >= begin) && (str[end] == ' '))
		end--;

	for (i = begin; i <= end; i++)
		str[i - begin] = str[i];

	str[i - begin] = '\0';
}

alloy_size alloy_strlen(const char *str)
{
	alloy_size i = 0;

	while (str[i] != 0)
		i++;

	return i;
}

#if 0

void list_files(struct AlloyShell *shell)
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
		alloy_term_set_foreground(&shell->term, 0x0000ff);
		alloy_term_write_asciiz(&shell->term, "Failed to read directory.\n");
		alloy_term_set_foreground(&shell->term, 0xffffff);
		return;
	}

	for (i = 0; i < 64; i++)
	{
		entry = &dir.Entries[i];
		if (bmfs_entry_is_empty(entry))
			continue;
		else if (bmfs_entry_is_terminator(entry))
			continue;
		else if (bmfs_entry_is_directory(entry))
		{
			alloy_term_write_asciiz(&shell->term, "Directory : ");
			alloy_term_set_foreground(&shell->term, 0xff0000);
			alloy_term_write_asciiz(&shell->term, entry->FileName);
			alloy_term_set_foreground(&shell->term, 0xffffff);
		}
		else
		{
			alloy_term_write_asciiz(&shell->term, "File      : ");
			alloy_term_write_asciiz(&shell->term, entry->FileName);
		}
		alloy_term_write_asciiz(&shell->term, "\t");
		printf_d(shell, entry->FileSize);
		alloy_term_write_asciiz(&shell->term, " bytes\n");
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
	void *app_data = (void *)0xFFFF800000000000;
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
		return -ENOENT;
	else if (bmfs_entry_is_directory(app_entry))
		return -EISDIR;

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

//	b_mem_allocate(&app_data, app_pages);

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

void alloy_overwrite_syscalls(struct AlloyShell *shell)
{
	syscall_term = &shell->term;
	*((uint64_t *)(0x100018)) = (uint64_t) &b_output_hook;
}

int alloy_output(const char *message, unsigned long long int message_length)
{
	return alloy_term_write(syscall_term, message, message_length);
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

void printf_d(struct AlloyShell *shell, int val)
{
	int f = 1, d;
	char digit;
	if (val < 0 && val != 0)
	{
		alloy_term_write(&shell->term, "-", 1);
		val = -val;
	}
	while((val/f) >= 10)
		f *= 10;
	while (f>0)
	{
		d = val/f;
		digit = '0' + d;
		alloy_term_write(&shell->term, &digit, 1);
		val = val-d*f;
		f = f/10;
	}
}

#endif
