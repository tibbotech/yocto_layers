/*
  test-freetype.c -- Measure fullscreen framebuffer write speed. 
  Version 0.5

  Copyright (C) 2003 Matthew Allum, Openedhand Ltd. 

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Matthew Allum mallum@openedhand.com

  ===
 
  Compile with

  gcc `freetype-config --libs --cflags` test-freetype.c -o test-freetype

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftsynth.h"

#define VERSION "0.1"

#define DEFAULT_TEXT_STR L"DimaДима迪瑪ديما"
//#define DEFAULT_TEXT_STR L"Dima"
//#define DEFAULT_FONT    "/usr/share/fonts/truetype/ttf-bitstream-vera/Vera.ttf"
//#define DEFAULT_FONT    "/usr/share/fonts/truetype/ARIALUNI.TTF.gz"
#define DEFAULT_FONT    "/usr/share/fonts/truetype/ARIALUNI.TTF"
#define DEFAULT_SIZE     20
#define DEFAULT_N_LINES  20
#define DEFAULT_N_CYCLES 100

static wchar_t *TextStr     = DEFAULT_TEXT_STR;
static char *TextFont    = DEFAULT_FONT;
static int   TextSize    = DEFAULT_SIZE;
static int   TextNLines  = DEFAULT_N_LINES;

static int con_fd = -1, fb_fd, last_vt = -1;
static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo vinfo;
static struct fb_cmap cmap;
static char  *fbuffer;
static int    fb_fd=0;
static int    xres, yres;
static int    Verbose = 0;
static int    TotalCycles = 100;

static char *defaultfbdevice = "/dev/fb0";
static char *defaultconsoledevice = "none";

static FT_Library FreetypeLibrary;

static int   WantMultiBlit = 0;

static int 
framebuffer_open (char *fbdevice,
		  char *consoledevice);
static void
framebuffer_blit(void);

static void 
framebuffer_close(void);

static unsigned long long
GetTimeInMillis(void)
{
  struct timeval  tp;

  gettimeofday(&tp, 0);
  return (unsigned long long)(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

static int 
framebuffer_open (char *fbdevice,
		  char *consoledevice)
{
  struct vt_stat vts;
  char vtname[128];
  int fd, nr;
  unsigned short col[2];
  int error;
  
  if (fbdevice == NULL) 
    fbdevice = defaultfbdevice;
  
  if (consoledevice == NULL) 
    consoledevice = defaultconsoledevice;
  
  if (strcmp(consoledevice,"none") != 0) 
    {
      sprintf(vtname,"%s%d", consoledevice, 1);
      fd = open(vtname, O_WRONLY);

      if (fd < 0) 
	{
	  perror("open consoledevice");
	  return -1;
	}
 
      /* Now get next available terminal */
      if (ioctl(fd, VT_OPENQRY, &nr) < 0) 
	{
	  perror("ioctl VT_OPENQRY");
	  return -1;
	}
      close(fd);  /* Close initial */
    
      /* Now open next available */
      sprintf(vtname, "%s%d", consoledevice, nr);
    
      con_fd = open(vtname, O_RDWR | O_NDELAY);
      if (con_fd < 0) 
	{
	  perror("open tty");
	  return -1;
	}
    
      /* Get info about it */
      if (ioctl(con_fd, VT_GETSTATE, &vts) == 0)
	last_vt = vts.v_active;
    
      /* Activate it */
      if (ioctl(con_fd, VT_ACTIVATE, nr) < 0) 
	{
	  perror("VT_ACTIVATE");
	  close(con_fd);
	  return -1;
	}
    
      /* Wait for it to activate */
      if (ioctl(con_fd, VT_WAITACTIVE, nr) < 0) 
	{
	  perror("VT_WAITACTIVE");
	  close(con_fd);
	  return -1;
	}
    
      /* Set the graphics mode */
      if (ioctl(con_fd, KDSETMODE, KD_GRAPHICS) < 0) 
	{
	  perror("KDSETMODE");
	  close(con_fd);
	  return -1;
	}
    }
  
  /* Now open the framebuffer */
  fb_fd = open(fbdevice, O_RDWR);
  if (fb_fd == -1) 
    {
      perror("open fbdevice");
      return -1;
    }

  /* Get the framebuffer info */
  if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix) < 0) 
    {
      perror("ioctl FBIOGET_FSCREENINFO");
      close(fb_fd);
      return -1;
    }
  
  if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) 
    {
      perror("ioctl FBIOGET_VSCREENINFO");
      close(fb_fd);
      return -1;
    }

  xres = vinfo.xres;
  yres = vinfo.yres;
  
  cmap.start = 0;
  cmap.len = 2;
  cmap.red = col;
  cmap.green = col;
  cmap.blue = col;
  cmap.transp = NULL;
  
  col[0] = 0;
  col[1] = 0xffff;
  
  /*
  if(var.bits_per_pixel==8) 
    {
      if (ioctl(fb_fd, FBIOPUTCMAP, &cmap) < 0) 
	{
	  perror("ioctl FBIOPUTCMAP");
	  close(fb_fd);
	  return -1;
	}
    }
  */

  if(vinfo.bits_per_pixel != 16)
    {
       perror("Framebuffer is not 16bpp.");
//      close(fb_fd);
//      return -1;
    }
printf( "%dx%d BPP: %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
printf( "mem len:%d\n", fix.smem_len);
    
  fbuffer = mmap(NULL, fix.smem_len, 
		 PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, 
		 fb_fd, 0);

  if (fbuffer == (char *)-1) 
    {
      perror("mmap framebuffer");
      close(fb_fd);
      return -1;
    }

  memset(fbuffer,0,fix.smem_len);

  /* freetype */

  error = FT_Init_FreeType( &FreetypeLibrary ); 
  
  if ( error ) {
    printf("Error in init\n");
    exit(2);
  }

  
  return 0;
}

static void 
framebuffer_close(void)
{
  munmap(fbuffer, fix.smem_len);
  close(fb_fd);
  
  if(con_fd != -1) 
    {
    
      if (ioctl(con_fd, KDSETMODE, KD_TEXT) < 0)
	perror("KDSETMODE");
    
      if (last_vt >= 0)
	if (ioctl(con_fd, VT_ACTIVATE, last_vt))
	  perror("VT_ACTIVATE");
    
      close(con_fd);
    }
}

static void
framebuffer_plot_pixel(int x, int y, u_int8_t _t)
{
 int poff = y * vinfo.xres + x;
 poff *= (vinfo.bits_per_pixel>>3);
//  int r = 0xff, g = 0xff, b =0xff;
//  int location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel/8) + ((y + vinfo.yoffset) * fix.line_length);
  
//  *((unsigned short int*)(fbuffer + location)) = 0xff; // (r<<11 | g << 5 | b);
  // transp | red | green | blue
//  *((u_int32_t*)(fbuffer + poff)) = 0xFF<<24 | 0xFF<<16 | 0xFF << 8 | 0xFF;
  *((u_int32_t*)(fbuffer + poff)) = ( _t)<<24 | 0xFF<<16 | 0xFF << 8 | 0xFF;

}

static void
framebuffer_blit(void)
{
  int x = 0, y = 200, i, j, xx, yy, nchars;
  char  *data = NULL;
  wchar_t *c;
  int    size = vinfo.yres * vinfo.xres * (vinfo.bits_per_pixel/8);
  unsigned long long start_clock,finish_clock,diff_clock;
  int     cycles = 0, error;
  FT_Face    face;

  data = malloc( size );

  start_clock = GetTimeInMillis(); 

  error = FT_New_Face( FreetypeLibrary, TextFont, 0, &face );

  if ( error == FT_Err_Unknown_File_Format ) {
    printf("Unknown font format\n");
    exit(-1);
  }

  if ( error == FT_Err_Cannot_Open_Stream ) {
    printf("Could not find/open font resource" );
    exit(-1);
  }

  if (error) {
    printf("Error %d occured, exiting %s\n", error, TextFont);
  }

  // FT_Set_Pixel_Sizes(face, 0, TextSize);

  /* Below assumes 72 dpi */
  error = FT_Set_Char_Size( face, 0, 30*64, 72, 72); 

  finish_clock = GetTimeInMillis();

  if (Verbose)
    {
      printf("test-freetype: Font loaded, drawable + color created in %lli ms\n",
	     finish_clock - start_clock);
    }

  start_clock = GetTimeInMillis(); 
  
  for (j=0; j<TotalCycles; j++)
    {
      y = 0;

      for (i=0; i<TextNLines; i++)
	{
	  c = &TextStr[0];

	  x = 0;

	  while (*c != '\0')
	    {
	      int glyph_index = FT_Get_Char_Index( face, (int)*c );

	      error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

	      if (error) {
		printf("WARNING: Glyph load error!\n");
	      }

	      if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP) 
		{
		  error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		  if (error) {
		    printf("WARNING: Glyph rendering error!\n");
		  }
		}

		for (yy=0; yy < face->glyph->bitmap.rows; yy++) 
		  for (xx=0; xx < face->glyph->bitmap.width; xx++) 
		    {
		      if (face->glyph->bitmap.buffer[yy*face->glyph->bitmap.width + xx] > 0) {
			  framebuffer_plot_pixel(100 + x + xx, 100 + y + yy - face->glyph->bitmap_top, face->glyph->bitmap.buffer[ yy*(face->glyph->bitmap.width) + xx]);
			}
		    }
		
		c++;
		x += face->glyph->bitmap.width + 2;
	    }

	  y += face->glyph->bitmap.rows + 12;

	}
    }
 
  finish_clock = GetTimeInMillis();

  diff_clock  = finish_clock - start_clock;

  nchars = wcslen(TextStr) * TextNLines * TotalCycles;

  printf("test-freetype: Total time %lli ms, %i glyphs rendered = approx %lli glyphs per second\n",
	 diff_clock, nchars, ( 1000 * nchars ) / diff_clock);
 
  free(data);
}


static void
usage(void)
{
  fprintf(stderr, 
	  "test-freetype " VERSION "\n"
	  "usage: test-freetype [options..]\n"
          "Options are;\n"
          "-display <X display>\n"
	  "--verbose\n"
	  "--text-str <str> text to render ( defaults to alphabet )\n"
	  "--font <str> ttf font to use ( defaults to " DEFAULT_FONT ")\n"
	  "--font-size <int> font size\n"
	  "--nlines <int> Number of lines to draw per cycle\n"
	  "--cycles <int>  number of times to runs the test ( default 100)\n"
	  );
  exit(1);
} 


int 
main (int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++) {


    if (!strcmp ("--verbose", argv[i]) || !strcmp ("-v", argv[i])) {
      Verbose = 1;
      continue;
    }

    if (!strcmp ("--text-str", argv[i]) ) {
      if (++i>=argc) usage ();
      TextStr = ( wchar_t *)argv[i];
      continue;
    }

    if (!strcmp ("--font", argv[i]) ) {
      if (++i>=argc) usage ();
      TextFont = argv[i];
      continue;
    }

    if (!strcmp ("--font-size", argv[i]) ) {
      if (++i>=argc) usage ();
      TextSize = atoi(argv[i]);
      continue;
    }

    if (!strcmp ("--nlines", argv[i]) ) {
      if (++i>=argc) usage ();
      TextNLines = atoi(argv[i]);
      if (TextNLines < 1) usage();
      continue;
    }

    if (!strcmp ("--cycles", argv[i])) {
      if (++i>=argc) usage ();
      TotalCycles = atoi(argv[i]);
      if (TotalCycles < 1) usage();
      continue;
    }

    usage();
  }

  if (framebuffer_open(NULL, NULL) == -1)
    {
      perror("framebuffer_open failed.");
      exit(0);
    }


  framebuffer_blit();

  framebuffer_close();

  return 0;
}
