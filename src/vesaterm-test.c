#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <alloy/vesaterm.h>

const char test_message[] = "Hello, world!";

int main(void)
{
	struct AlloyVesaTerm vesaterm;

	alloy_vesaterm_init(&vesaterm);

	struct AlloyTerm *term = &vesaterm.base;

	int err = alloy_term_write(term, test_message, sizeof(test_message) - 1);
	assert(err == 0);

	/* dump memory into a bitmap file,
	 * to be exampled for aesthetics */

	/* used http://www.dragonwins.com/domains/getteched/bmp/bmpfileformat.htm
	 * for bitmap file format reference */

	FILE *bitmap = fopen("vesaterm-test.bmp", "wb");
	if (bitmap == NULL)
	{
		fprintf(stderr, "Failed to open bitmap file.\n");
		return EXIT_FAILURE;
	}

	/* .bmp magic number */
	fwrite("BM", 2, 1, bitmap);
	/* file size, in bytes */
	unsigned int file_size = 0;
	/* frame buffer size */
	file_size += vesaterm.x_res * vesaterm.y_res;
	/* file header and image header sizes */
	file_size += 12 + 40;
	/* write the file size (TODO : should this be little or big endian?) */
	fwrite(&file_size, 4, 1, bitmap);
	/* reserved bytes */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* pixel data offset, in bytes */
	unsigned int pixels_offset = 0;
	/* file header and image header sizes. */
	pixels_offset += 12 + 40;
	/* write pixel data offset */
	fwrite(&pixels_offset, 4, 1, bitmap);
	/* image header size, in bytes (always 40) */
	unsigned int image_header_size = 40;
	/* write image header size */
	fwrite(&image_header_size, 4, 1, bitmap);
	/* image width */
	fwrite(&vesaterm.x_res, 4, 1, bitmap);
	/* image height */
	fwrite(&vesaterm.y_res, 4, 1, bitmap);
	/* image planes (always 1) */
	fwrite("\x01\x00", 2, 1, bitmap);
	/* color depth, in bits (always 24) */
	fwrite("\x10\x00", 2, 1, bitmap);
	/* compression (always none) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* image size (zero for uncompressed images) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* preferred horizontal resolution (zero means no preference) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* preferred vertical resolution (zero means no preference) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* color map entries used (none) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* number of significant colors (zero means all of them) */
	fwrite("\x00\x00\x00\x00", 4, 1, bitmap);
	/* pixel data */
	fwrite(vesaterm.frame_buffer, 1, vesaterm.x_res * vesaterm.y_res, bitmap);
	/* done */
	fclose(bitmap);

	alloy_term_done(term);

	return EXIT_SUCCESS;
}
