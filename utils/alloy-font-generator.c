/* Alloy - The BareMetal middleware */
/* Copyright 2017 Return Infinity, Inc */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ft2build.h>
#include FT_FREETYPE_H

static void export_header(FILE *outfile, unsigned int glyph_count)
{
	fprintf(outfile, "/* Warning: This file is automatically generated.\n");
	fprintf(outfile, " *          Any changes to this file may be overwritten.\n");
	fprintf(outfile, " */\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "#include <alloy/font.h>\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "struct AlloyGlyph alloy_glyphs[%u] = {\n", glyph_count);
}

static void export_glyph(FILE *outfile, char ascii_value, FT_GlyphSlotRec *glyph_slot)
{
	fprintf(outfile, "\t{\n");
	if ((ascii_value == '\'')
	 || (ascii_value == '\"')
	 || (ascii_value == '\\'))
		fprintf(outfile, "\t\t'\\%c',\n", ascii_value);
	else
		fprintf(outfile, "\t\t'%c',\n", ascii_value);
	fprintf(outfile, "\t\t%u,\n", glyph_slot->bitmap.width);
	fprintf(outfile, "\t\t%u,\n", glyph_slot->bitmap.rows);
	fprintf(outfile, "\t\t%u,\n", glyph_slot->bitmap_left);
	fprintf(outfile, "\t\t%u,\n", glyph_slot->bitmap_top);
	fprintf(outfile, "\t\t%lu,\n", glyph_slot->advance.x >> 6);
	/* export bitmap data */
	unsigned int width = glyph_slot->bitmap.width;
	unsigned int height = glyph_slot->bitmap.rows;
	unsigned int pitch = glyph_slot->bitmap.pitch;
	unsigned char *buffer = glyph_slot->bitmap.buffer;
	if ((height == 0) || (width == 0))
	{
		fprintf(outfile, "\t\t(const unsigned char *) 0x00,\n");
	}
	else
	{
		fprintf(outfile, "\t\t(const unsigned char *) \"");
		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
				fprintf(outfile, "\\x%02x", buffer[(y * pitch) + x]);

			fprintf(outfile, "\"\n");

			if ((y + 1) < height)
				fprintf(outfile, "\t\t                        \"");
		}
	}
	fprintf(outfile, "\t},\n");
}

static void export_footer(FILE *outfile, FT_Face face)
{
	fprintf(outfile, "};\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "const struct AlloyFont alloy_font = {\n");
	fprintf(outfile, "\t\"%s\",\n", face->family_name);
	fprintf(outfile, "\t\"%s\",\n", face->style_name);
	fprintf(outfile, "\t%u,\n", face->height >> 6);
	fprintf(outfile, "\t%u,\n", face->ascender >> 6);
	fprintf(outfile, "\t%u,\n", face->descender >> 6);
	fprintf(outfile, "\talloy_glyphs,\n");
	fprintf(outfile, "\tsizeof(alloy_glyphs) / sizeof(alloy_glyphs[0])\n");
	fprintf(outfile, "};\n");
	fprintf(outfile, "\n");
	fprintf(outfile, "const struct AlloyGlyph *alloy_font_get_glyph(const struct AlloyFont *font, unsigned int c)\n");
	fprintf(outfile, "{\n");
	fprintf(outfile, "\tfor (unsigned int i = 0; i < font->glyph_count; i++)\n");
	fprintf(outfile, "\t{\n");
	fprintf(outfile, "\t\tif (font->glyph_array[i].ascii_value == c)\n");
	fprintf(outfile, "\t\t\treturn &font->glyph_array[i];\n");
	fprintf(outfile, "\t}\n");
	fprintf(outfile, "\treturn 0;\n");
	fprintf(outfile, "}\n");
}

struct alloy_font_plan
{
	const char *fontfile;
	const char *outfile;
	unsigned int font_size;
	unsigned int dpi_x;
	unsigned int dpi_y;
	const char *glyph_array;
	unsigned int glyph_count;
};

static void alloy_font_plan_init(struct alloy_font_plan *font_plan)
{
	font_plan->fontfile = "../fonts/source-code-pro/source-code-pro-2.030R-ro-1.050R-it/TTF/SourceCodePro-Regular.ttf";
	font_plan->outfile = "font.c";
	font_plan->font_size = 16;
	font_plan->dpi_x = 72;
	font_plan->dpi_y = 72;
	font_plan->glyph_array = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	font_plan->glyph_count = strlen(font_plan->glyph_array);
}

static int alloy_font_plan_execute(struct alloy_font_plan *font_plan)
{
	FILE *outfile = fopen(font_plan->outfile, "wb");
	if (outfile == NULL)
	{
		fprintf(stderr, "Failed to open output file '%s'.\n", font_plan->outfile);
		return EXIT_FAILURE;
	}

	export_header(outfile, font_plan->glyph_count);

	FT_Library library;

	int err = FT_Init_FreeType(&library);
	if (err != 0)
	{
		fprintf(stderr, "Failed to initialize FreeType library.\n");
		fclose(outfile);
		return EXIT_FAILURE;
	}

	FT_Face face;

	err = FT_New_Face(library, font_plan->fontfile, 0, &face);
	if (err != 0)
	{
		fprintf(stderr, "Failed to open font face: %s.\n", font_plan->fontfile);
		FT_Done_FreeType(library);
		fclose(outfile);
		return EXIT_FAILURE;
	}

	err = FT_Set_Pixel_Sizes(face, 0, font_plan->font_size);
	if (err != 0)
	{
		FT_Done_FreeType(library);
		fclose(outfile);
		return EXIT_FAILURE;
	}

	for (unsigned int i = 0; i < font_plan->glyph_count; i++)
	{
		char c = font_plan->glyph_array[i];

		FT_UInt glyph_index = FT_Get_Char_Index(face, c);
		if (glyph_index == 0)
		{
			fprintf(stderr, "Unsupported glyph: '%c'.\n", c);
			FT_Done_FreeType(library);
			fclose(outfile);
			return EXIT_FAILURE;
		}

		err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (err != 0)
		{
			fprintf(stderr, "Failed to load glyph: '%c'.\n", c);
			FT_Done_FreeType(library);
			fclose(outfile);
			return EXIT_FAILURE;
		}

		err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if (err != 0)
		{
			fprintf(stderr, "Failed to render glyph: '%c'.\n", c);
			FT_Done_FreeType(library);
			fclose(outfile);
			return EXIT_FAILURE;
		}

		export_glyph(outfile, c, face->glyph);
	}

	export_footer(outfile, face);

	FT_Done_FreeType(library);

	fclose(outfile);

	return EXIT_SUCCESS;
}

static void print_usage(const char *argv0)
{
	printf("Usage: %s [options]\n", argv0);
	printf("\n");
	printf("Options:\n");
	printf("\t--help      : Print this help message and exit.\n");
	printf("\t--font FILE : Chose a font for Alloy.\n");
}

int main(int argc, const char **argv)
{
	struct alloy_font_plan font_plan;

	alloy_font_plan_init(&font_plan);

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--font") == 0)
		{
			font_plan.fontfile = argv[i++];
		}
		else if (strcmp(argv[i], "--help") == 0)
		{
			print_usage(argv[0]);
			return EXIT_FAILURE;
		}
		else
		{
			fprintf(stderr, "Unknown option '%s'.\n", argv[i]);
			return EXIT_FAILURE;
		}
	}

	return alloy_font_plan_execute(&font_plan);
}
