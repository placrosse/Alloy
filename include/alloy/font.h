#ifndef ALLOY_FONT_H
#define ALLOY_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct AlloyGlyph
{
	unsigned int ascii_value;
	unsigned int width;
	unsigned int height;
	unsigned int left;
	unsigned int top;
	const unsigned char *buf;
};

struct AlloyFont
{
	const struct AlloyGlyph *glyph_array;
	unsigned char glyph_count;
};

extern const struct AlloyFont alloy_font;

const struct AlloyGlyph *alloy_font_get_glyph(const struct AlloyFont *font, unsigned int c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_FONT_H */
