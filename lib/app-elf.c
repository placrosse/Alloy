/* ===============================================================
 * Alloy - A platform-independent terminal and shell program.
 * Copyright (C) 2017 - 2018 Return Infinity
 * See LICENSE for license information.
 * ===============================================================
 */

#include <alloy/app.h>

#include <alloy/errno.h>
#include <alloy/string.h>

enum objfile_type
{
	objfile_executable,
	objfile_relocatable
};

int alloy_app_load_elf(struct AlloyApp *app,
                       const void *app_data_ptr,
                       alloy_size app_size)
{
	void *load_addr = (void *) 0xffff800000000000;

	const unsigned char *app_data = (const unsigned char *) app_data_ptr;

	if (app_size < 64)
		return ALLOY_EINVAL;

	/* Check signature */
	if ((app_data[0] != 0x7f)
	 || (app_data[1] != 'E')
	 || (app_data[2] != 'L')
	 || (app_data[3] != 'F'))
		return ALLOY_EINVAL;

	/* Check class (2 == 64 bit) */
	if (app_data[4] != 2)
		return ALLOY_EINVAL;

	/* Check endianness (1 == little) */
	if (app_data[5] != 1)
		return ALLOY_EINVAL;

	/* Check ELF version */
	if (app_data[6] != 1)
		return ALLOY_EINVAL;

	/* Check OS ABI (should be System V) */
	if (app_data[7] != 0)
		return ALLOY_EINVAL;

	enum objfile_type type;

	/* Check object file type */
	if ((app_data[16] == 0x02)
	 && (app_data[17] == 0x00))
		type = objfile_executable;
	else if ((app_data[16] == 0x01)
	      && (app_data[17] == 0x00))
		type = objfile_relocatable;
	else
		return ALLOY_EINVAL;

	if (type == objfile_relocatable)
		return ALLOY_EINVAL;

	/* Get the entry point. */

	app->entry = (alloy_app_entry) *((void **) &app_data[24]);

	/* Get the fields for parsing the program headers. */

	unsigned long int phoff = *(unsigned long int *) &app_data[32];

	unsigned short int phentsize = *(unsigned short int *) &app_data[54];

	unsigned short int phnum = *(unsigned short int *) &app_data[56];

	/* Check for possible overflow. */

	if ((phoff + (phentsize * phnum)) > app_size)
		return ALLOY_EINVAL;

	/* Parse the program headers. */

	for (unsigned int i = 0; i < phnum; i++)
	{
		const unsigned char *ph = &app_data[phoff + (i * phentsize)];
		/* See if the program header is loadable. */
		if ((ph[0] != 0x01)
		 || (ph[1] != 0x00)
		 || (ph[2] != 0x00)
		 || (ph[3] != 0x00))
			continue;

		/* Get the address of the program header. */
		void *ph_addr = (void *)(*((void **) &ph[16]));

		/* Ensure that the load address is in the proper range. */
		if (ph_addr < load_addr)
			return ALLOY_EINVAL;

		/* The the address of the program data. */
		unsigned long int ph_offset = *((unsigned long int *) &ph[8]);

		unsigned long int ph_size = (unsigned long int)(*((unsigned long int *) &ph[32]));

		alloy_memcpy(ph_addr, &app_data[ph_offset], ph_size);
	}

	return 0;
}
