#include <FL/Fl_BMP_Image.H>
#include <png.h>
#include <stdio.h>
#include <lrtypes.h>

static void savepng(const u8 * const data, FILE * const f, const u32 w, const u32 h) {

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) abort();
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) abort();
	if (setjmp(png_jmpbuf(png_ptr))) abort();

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, w, h, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info);

	u32 i;
	for (i = 0; i < h; i++) {
		png_write_row(png_ptr, (png_byte *) data + i * w * 3);
	}
	png_write_end(png_ptr, NULL);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_write_struct(&png_ptr, NULL);
}

int main(int argc, char **argv) {

	u32 i, w, h;
	const char * const outname = argv[argc - 1];

	if (argc < 4) {
		printf("Usage: %s WxH files.. out.png\n",
			argv[0]);
		return 1;
	}

	if (sscanf(argv[1], "%ux%u", &w, &h) != 2) {
		printf("Dimensions invalid\n");
		return 1;
	}

	printf("%ux%u tiles, %u argc.\n", w, h, argc);
	u8 * const data = (u8 *) calloc(w * 8 * h * 8, 3);

	const u32 dw = w * 8 * 3;
	u32 t = 0;
	for (i = 2; i < (u32) argc - 1; i++, t++) {
		Fl_BMP_Image img(argv[i]);
		if (img.d() != 3) {
			printf("Wrong depth\n");
			return 1;
		}

		if (img.w() != 8 || img.h() != 8) {
			printf("Wrong dimensions\n");
			return 1;
		}

		// Copy it in
		const u8 * const src = img.array;
		u8 y;
		const u32 row = t / w;
		const u32 col = t % w;

		for (y = 0; y < 8; y++) {
			memcpy(data + row * dw + col * 8 * 3,
				src + y * 8 * 3, 8 * 3);
		}
	}

	FILE *f = fopen(outname, "w");
	if (!f) {
		printf("Can't open %s\n", outname);
		return 1;
	}

	savepng(data, f, w * 8, h * 8);

	free(data);
	fclose(f);
	return 0;
}
