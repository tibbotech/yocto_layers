#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
//#include <hdmitx.h>
#include <unistd.h>
#include <ft2build.h>
#include <glib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <hb.h>
#include <hb-ft.h>

#include FT_FREETYPE_H

char *framebuffer;

struct fb_fix_screeninfo fixinfo;
struct fb_var_screeninfo varinfo;

int draw_glyph(FT_Face face, unsigned int glyphid, unsigned int xpos, unsigned int ypos){
    int error;
    int startx = 10, starty = 100;

    error = FT_Load_Glyph(face, glyphid, FT_LOAD_DEFAULT);
    if ( error) printf( "%d %d\n", __LINE__, error);

    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if ( error) printf( "%d %d\n", __LINE__, error);

    FT_GlyphSlot slot = face->glyph;

    unsigned int color = 0xFF00e380;
    unsigned int *dest = (unsigned int *)framebuffer;

    for(unsigned int j = 0; j < slot->bitmap.rows; j++){
        for(unsigned int i = 0; i < slot->bitmap.width; i++){
            unsigned int pix = (unsigned int)slot->bitmap.buffer[j*slot->bitmap.pitch+i];


            unsigned int val = 0xFF000000 | pix << 16 | pix << 8 | pix;
            val &= color;

            int y_coord = starty+j-slot->bitmap_top;
            if(y_coord < 0){
                y_coord = 0;
            }
            unsigned int offset = (y_coord)*varinfo.xres+i+xpos+startx;
            dest[offset] |= val;
        }
    }

    return 0;
}

int render(const char *text){
    int error;

    unsigned int glyph_count;

    FT_Library library;
    FT_Face face;

    hb_buffer_t *buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, -1, 0, -1);

    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_HAN);
    hb_buffer_set_language(buf, hb_language_from_string("ch", -1));

    error = FT_Init_FreeType(&library);
    if ( error) printf( "%d %d\n", __LINE__, error);

    error = FT_New_Face(library, "/usr/share/fonts/truetype/ARIALUNI.TTF",0, &face);
    if ( error) printf( "%d %d\n", __LINE__, error);

    /**
    Char dimensions are defined in 1/64 of pts or ~0.2170 px if resolution is 1000dpi
    **/

    FT_Set_Char_Size(face, 0, 30/0.2170, 1000, 1000);
    hb_font_t *font = hb_ft_font_create(face, NULL);

    hb_shape(font, buf, NULL, 0);

    hb_glyph_info_t *glyph_info    = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    unsigned int glyphid, x_offset = 0, y_offset = 0, x_advance = 0, y_advance = 0, cursor_x = 0, cursor_y = 0;

    for (unsigned int i = 0; i < glyph_count; ++i) {
        glyphid = glyph_info[i].codepoint;
        x_offset = (unsigned int)(glyph_pos[i].x_offset / 64.0);
        y_offset = (unsigned int)(glyph_pos[i].y_offset / 64.0);
        x_advance = (unsigned int)(glyph_pos[i].x_advance / 64.0);
        y_advance = (unsigned int)(glyph_pos[i].y_advance / 64.0);

printf( "x_off:%d y_off:%d\n", x_offset, y_offset);
printf( "x_adv:%d y_adv:%d\n", x_advance, y_advance);
        draw_glyph(face, glyphid, cursor_x + x_offset, cursor_y + y_offset);

        cursor_x += x_advance;
        cursor_y += y_advance;
    }

    hb_buffer_destroy(buf);
    hb_font_destroy(font);

    return 0;
}

int init_framebuffer(){
    int fb_fd = open("/dev/fb0", O_RDWR);
    if ( fb_fd < 0) printf( "%d %d", __LINE__, fb_fd);

    int res;

    res = ioctl (fb_fd, FBIOGET_FSCREENINFO, &fixinfo);
    printf("[FB] Obtainig fixed screen info: 0x%x\n", res);

    res = ioctl (fb_fd, FBIOGET_VSCREENINFO, &varinfo);
    printf("[FB] Obtainig variable screen info: 0x%x\n", res);

    framebuffer = mmap (0, fixinfo.smem_len, PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if (framebuffer == MAP_FAILED){
        printf("[FB] Unable to map memory\n");
        return -1;
    }

    unsigned int *dest = (unsigned int *)framebuffer;

//    for(unsigned int j = 0; j < varinfo.yres; j++){
//        for(unsigned int i = 0; i < varinfo.xres; i++){
//            dest[j*varinfo.xres+i] = 0xFF000000;
//        }
//    }

    printf("[FB] Framebuffer info. W: %i, H: %i\n", varinfo.xres, varinfo.yres);

    return 0;
}

int main() {
//    init_hdmi();
    init_framebuffer();
    render( "DimaДима迪瑪ديما");
//    render("或被視為隸屬於漢藏語系漢語族之一種語言。");

}
