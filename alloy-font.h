#ifndef ALLOY_FONT_H
#define ALLOY_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct alloy_glyph
{
	unsigned int ascii_value;
	unsigned int width;
	unsigned int height;
	unsigned int left;
	unsigned int top;
	const unsigned char *buf;
};

struct alloy_font
{
	const struct alloy_glyph *glyph_array;
	unsigned char glyph_count;
};

extern const struct alloy_font alloy_font;

const struct alloy_glyph *alloy_font_get_glyph(const struct alloy_font *font, unsigned int c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_FONT_H */
