#ifndef ALLOY_FONT_H
#define ALLOY_FONT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Used to contain a glyph that is generated
 * for Alloy at compile time.
 * */

struct AlloyGlyph
{
	/** The ASCII character associated with
	 * the glyph. */
	unsigned int ascii_value;
	/** The width of the bitmap. */
	unsigned int width;
	/** The height of the bitmap. */
	unsigned int height;
	/** The horizontal offset that the bitmap
	 * should begin, relative to the pen position. */
	unsigned int left;
	/** The vertical offset that the bitmap should
	 * begin, relative to the pen position. */
	unsigned int top;
	/** The number of pixels the pen should move
	 * horizontally, after a character is printed. */
	unsigned int advance;
	/** A grayscale bitmap of the glyph. */
	const unsigned char *buf;
};

/** Used to contain a font face that is generated
 * for Alloy at compile time.
 * */

struct AlloyFont
{
	/** The name of the font family. */
	const char *name;
	/** The name of the font style.*/
	const char *style;
	/** The number of pixels that a line of text
	 * should occupy, vertically. */
	unsigned int line_height;
	/** From the baseline to the top of the
	 * font face, in pixels. */
	unsigned int ascender;
	/** From the baseline to the botton of the
	 * font face, in pixels. */
	unsigned int descender;
	/** The array of glyphs in the font. */
	const struct AlloyGlyph *glyph_array;
	/** The number of glyphs in the glyph array. */
	unsigned char glyph_count;
};

/** The compile-time generated font face for Alloy.
 * */

extern const struct AlloyFont alloy_font;

/** Used for accessing a glyph from the compile-time
 * generated font for Alloy.
 * */

const struct AlloyGlyph *alloy_font_get_glyph(const struct AlloyFont *font, unsigned int c);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ALLOY_FONT_H */
