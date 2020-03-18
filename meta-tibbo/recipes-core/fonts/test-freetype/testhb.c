#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <linux/fb.h>
#include <wchar.h>

#define USE_HB 0

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftsynth.h"

#if USE_HB
#include <hb.h>
#include <hb-ft.h>
#endif

#define FONT_SIZE 36

#define DEFAULT_FONT "/usr/share/fonts/truetype/ARIALUNI.TTF"
#define DEFAULT_TEXT_STR L"DimaДима迪瑪ديما"
#define DEFAULT_FB "/dev/fb0"

static int    fb_fd=0;
static char  *fbuffer;
static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo vinfo;

static unsigned long long GetTimeInMillis(void) {
 struct timeval  tp;
 gettimeofday(&tp, 0);
 return (unsigned long long)(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

// _GRAY value is 1byte. Used as transparency to have better view
static void FB_pix( u_int16_t x, u_int16_t y, u_int8_t _trans) {
 int poff = y * vinfo.xres + x;
 poff *= (vinfo.bits_per_pixel>>3);
 if ( poff > fix.smem_len) return;
 *((u_int32_t*)(fbuffer + poff)) = (_trans)<<24 | 0xFF<<16 | 0xFF << 8 | 0xFF;
// *((u_int32_t*)(fbuffer + poff)) = 0xFF<<24 | 0xFF<<16 | 0xFF << 8 | 0xFF;
}

static int FB_open( char *_dev) {
 int fd;
  
 if ( !_dev) _dev = "/dev/fb0";
 if ( ( fb_fd = open( _dev, O_RDWR)) < 0) {
   printf( "%s: %s\n", _dev, strerror( errno));
   return( -1);  }
 /* Get the framebuffer info */
 if ( ioctl( fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) {
   perror("ioctl FBIOGET_FSCREENINFO");
   close(fb_fd);
   return -1;  }
 if ( ioctl( fb_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
   perror("ioctl FBIOGET_VSCREENINFO");
   close(fb_fd);
   return -1;  }
 printf( "%dx%d BPP: %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
 printf( "mem len:%d\n", fix.smem_len);
 fbuffer = mmap( NULL, fix.smem_len, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fb_fd, 0);
 if ( fbuffer == (char *)-1) {
   printf( "mmap: %s\n", strerror( errno));
   close( fb_fd);
   return( -1);  }
 memset( fbuffer, 0, fix.smem_len);
 return( 0);  }

FT_GlyphSlot FB_glyph_get( FT_Face _font, int _gidx) {
 int err;
 FT_GlyphSlot gs;
 if ( FT_Load_Glyph( _font, _gidx, FT_LOAD_DEFAULT)) {
   printf( "FT_Load error\n");  return( NULL);  }
 gs = _font->glyph;
 if ( gs->format != FT_GLYPH_FORMAT_BITMAP) {
   if ( FT_Render_Glyph( gs, FT_RENDER_MODE_NORMAL)) {
   	 printf( "FT_Render error\n");  return( NULL);  }
 }
 if ( gs->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
   printf( "Can't handle pixel mode == %d\n", gs->bitmap.pixel_mode);
   return( NULL);  }
 return( gs);  }

int draw_G( FT_Face _font, int _gidx, int _x, int _y) {
 int err, x, y;
 FT_GlyphSlot gs;
 if ( FT_Load_Glyph( _font, _gidx, FT_LOAD_DEFAULT)) {
   printf( "FT_Load error\n");  return( -1);  }
 gs = _font->glyph;
 if ( gs->format != FT_GLYPH_FORMAT_BITMAP) {
   if ( FT_Render_Glyph( gs, FT_RENDER_MODE_NORMAL)) {
   	 printf( "FT_Render error\n");  return( -1);  }
 }
 if ( gs->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
   printf( "Can't handle pixel mode == %d\n", gs->bitmap.pixel_mode);
   return( -1);  }
// printf( "%d pitch:%d bt:%d \n", _gidx, gs->bitmap.pitch, gs->bitmap_top);
 for ( int y = 0; y < gs->bitmap.rows; y++) {
   for ( int x = 0; x < gs->bitmap.width; x++) {
     if ( gs->bitmap.buffer[ y*(gs->bitmap.width) + x] < 1) continue;
     FB_pix( _x + x, _y - gs->bitmap_top + y, gs->bitmap.buffer[ y*(gs->bitmap.width) + x]);
   }
 }
// printf( "\n");
 return( 1);  }

int FB_set_font_sz( FT_Face _font, u_int16_t _sz) {
 // 0,0 - hres, vres in dpi ?
 if ( FT_Set_Char_Size( _font, _sz*64, _sz*64, 0, 0) == 0) return( 1);
 printf( "FT_New_Face problem\n");
 return( -1);  }
    
int main(int argc, char **argv) {
  const wchar_t *text, *c;
  int error;
  unsigned long long start_clock,finish_clock,diff_clock;

  text = DEFAULT_TEXT_STR;
  FB_open( DEFAULT_FB);

  start_clock = GetTimeInMillis(); 

  /* Initialize FreeType and create FreeType font face. */
  FT_Library ft_library;
  FT_Face ft_face;
  FT_Error ft_error;

  if ( FT_Init_FreeType( &ft_library)) {
    printf( "FT_Init problem\n");  return( -1);  }
  if ( FT_New_Face( ft_library, DEFAULT_FONT, 0, &ft_face)) {
    printf( "FT_New_Face problem %s\n", DEFAULT_FONT);  return( -1);  }
 if ( FB_set_font_sz( ft_face, FONT_SIZE) < 0) return( -1);

#if USE_HB
  /* Create hb-ft font. */
  hb_font_t *hb_font;
  hb_font = hb_ft_font_create (ft_face, NULL);

  /* Create hb-buffer and populate. */
  hb_buffer_t *hb_buffer;
  hb_buffer = hb_buffer_create ();
  hb_buffer_add_utf32 (hb_buffer, DEFAULT_TEXT_STR, -1, 0, -1);
  hb_buffer_guess_segment_properties (hb_buffer);

  /* Shape it! */
  hb_shape (hb_font, hb_buffer, NULL, 0);

  /* Get glyph information and positions out of the buffer. */
  unsigned int len = hb_buffer_get_length (hb_buffer);
  hb_glyph_info_t *info = hb_buffer_get_glyph_infos (hb_buffer, NULL);
  hb_glyph_position_t *pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);
#endif
/////
  double current_x = 100;
  double current_y = 300;
  int gindex, i = 0, j = 0;
  c = text;
#if USE_HB
  for ( i = 0; i < len; i++) {
#else
 while ( *c != '\0') {
#endif
     gindex = FT_Get_Char_Index( ft_face, (int)*c );
     draw_G( ft_face, gindex, current_x, current_y);
#if USE_HB
     current_x += pos[i].x_advance / 64.;
     current_y += pos[i].y_advance / 64.;
#else
printf( "%d x:%d w:%d h:%d\n", gindex, ft_face->glyph->advance.x/64, ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows);
     // advance = width + space
     current_x += ft_face->glyph->advance.x/64;
     current_y += ft_face->glyph->advance.y/64;
#endif
     c++;  }
//////
// test
 FT_GlyphSlot gs;
 int a_w = 0, a_h = 0;
 c = text;
 while ( *c != '\0') {
    gindex = FT_Get_Char_Index( ft_face, (int)*c );
    gs = FB_glyph_get( ft_face, gindex);
    a_w += ( gs->advance.x ? gs->advance.x/64 : gs->bitmap.width);
    a_h += ( gs->advance.y ? gs->advance.y/64 : gs->bitmap.rows);
    c++;
 }
printf( "a_w:%d a_h:%d\n", a_w, a_h);
//////
#if USE_HB
  hb_buffer_destroy (hb_buffer);
  hb_font_destroy (hb_font);
#endif
  
  FT_Done_Face (ft_face);
  FT_Done_FreeType (ft_library);

  finish_clock = GetTimeInMillis();
  munmap( fbuffer, fix.smem_len);
  close( fb_fd);
  printf( "Done in %lli ms\n", finish_clock - start_clock);
  return 0;
}
