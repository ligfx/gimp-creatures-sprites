/* Creatures Sprites Plug-in
 * Copyright (C) 2001 Tina Hirsch <tehirsch@nexgo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "main.h"

#include "plugin-intl.h"

#include <c16/sprite.h>
#include <c16/image.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef BLK_BLOCK_SIZE
#define BLK_BLOCK_SIZE 128
#endif



/*  Local function prototypes  */

static void   query (void);
static void   run   (gchar      *name,
		     gint        nparams,
		     GimpParam  *param,
		     gint       *nreturn_vals,
		     GimpParam **return_vals);
static gint32 load_blkimage (char *);
static gint save_blkimage (char *, gint32, gint32);

/* Structures */
typedef struct
{
  gint type555;
} RGBType;



/*  Local variables  */

RGBType savevals =
  {
    FALSE
  };



GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};

MAIN ()

static void
query (void)
{
  
  /* define the input arguments for the database procedure */
  static GimpParamDef loadblk_args[] =
  {
    { GIMP_PDB_INT32,    "run_mode",   "Interactive, non-interactive" },
    { GIMP_PDB_STRING,   "filename",    "The name of the file to load" },
    { GIMP_PDB_STRING,   "raw_filename",  "The name of the file to load" }
  };

  static GimpParamDef saveblk_args[] =
    {
      { GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive" },
      { GIMP_PDB_IMAGE, "image", "Input image" },
      { GIMP_PDB_DRAWABLE, "drawable", "Drawable to save" },
      { GIMP_PDB_STRING, "filename", "The name of the file to save the image" },
      { GIMP_PDB_STRING, "raw_filename", "The name of the file to save the image" },
      { GIMP_PDB_INT32, "type555", "Currently not supported" }
    };

  /* return values */
  static GimpParamDef loadblk_return_vals[]=
    {
      { GIMP_PDB_IMAGE, "image", "Output image" }
    };

  /* count of the input args */
  static gint nloadblk_args = sizeof (loadblk_args) / sizeof (loadblk_args[0]);

  static gint nsaveblk_args = sizeof (saveblk_args) / sizeof (saveblk_args[0]);
  
  /* count of the return values */
  static int nloadblk_return_vals = sizeof(loadblk_return_vals) /
  sizeof(loadblk_return_vals[0]);

  /* register help and locales */
  gimp_plugin_domain_register (PLUGIN_NAME, LOCALEDIR);
  gimp_plugin_help_register (DATADIR"/help");

  /* register the procedures in the database */
  gimp_install_procedure ("file_blk_load",
                          _("Loads files in blk image format"),
                          _("This plug-in loads Creatures backgroundimage files"),
                          "Tina Hirsch",
                          "Tina Hirsch",
                          "2001",
                          "<Load>/BLK",
                          NULL,
                          GIMP_PLUGIN,
                          nloadblk_args,
                          nloadblk_return_vals,
                          loadblk_args,
                          loadblk_return_vals);

  gimp_install_procedure ("file_blk_save",
                          _("Saves files in blk image format"),
                          _("This plug-in saves Creatures backgroundimage files"),
                          "Tina Hirsch",
                          "Tina Hirsch",
                          "2001",
                          "<Save>/BLK",
                          "RGB",
                          GIMP_PLUGIN,
                          nsaveblk_args,
                          0,
                          saveblk_args,
                          NULL);

  /* register the handlers for saving and loading images */
  gimp_register_load_handler ("file_blk_load", "blk", "");
  gimp_register_save_handler ("file_blk_save", "blk", "");
}

static void
run (gchar      *name, /* I - Name of filter program */
     gint        n_params, /* I -Number of parameters passed in */
     GimpParam  *param, /* I - Parameter values */
     gint       *nreturn_vals, /* O - Number of return values */
     GimpParam **return_vals) /* O - Return values */
{
  GimpParam   *values; /* return values */
  gint32 image_ID;
  GimpRunModeType run_mode;
  gint32 drawable_ID;
  GimpDrawable *drawable;
  gint success;
  
  success = 0;
  
  values = g_new (GimpParam, 2);
  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_SUCCESS;

  *return_vals  = values;

  drawable_ID = param[2].data.d_int32;
  drawable = gimp_drawable_get (drawable_ID);
  run_mode = param[0].data.d_int32; 

  /* Load a blkimage.... */
  if (strcmp (name, "file_blk_load") == 0)
    {
      *nreturn_vals = 2;

      image_ID = load_blkimage (param[1].data.d_string);

      if( image_ID != -1)
        {
          values[1].type = GIMP_PDB_IMAGE;
          values[1].data.d_image = image_ID;
        }
      else
        values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    }
  /* ... or save a blkimage? */
  else if(strcmp (name, "file_blk_save") == 0)
    {
      *nreturn_vals=1;
      
      switch (run_mode) 
        {
          case GIMP_RUN_NONINTERACTIVE:
            if (n_params != 6)
              values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
            else
              savevals.type555 = param[5].data.d_int32; 
            break;

          case GIMP_RUN_INTERACTIVE:
            /*  Possibly retrieve data */
             gimp_get_data ("file_blk_save", &savevals); 
             break; 

          case GIMP_RUN_WITH_LAST_VALS: 
             /*  Possibly retrieve data  */ 
            gimp_get_data ("file_blk_save" , &savevals); 
        }
      
      if(values[0].data.d_status == GIMP_PDB_SUCCESS)
        {
          success = save_blkimage (param[3].data.d_string, param[1].data.d_int32,
                                   param[2].data.d_int32);
          if(success == 1)
            gimp_set_data ("file_blk_save", &savevals, sizeof(savevals));
          else if (success ==2)
            values[0].data.d_status = GIMP_PDB_CANCEL;
          else
            values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
        }
    }
  else
    values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
}

static gint load_blkimage (char *filename)
{
  int width, height;
  int i;
  
  gint32 image;
  gint32 layer;
  GimpDrawable *drawable;
  GimpPixelRgn pixel_rgn;
  guchar **pixel; /* the pixel data */
  C16Sprite_p sprite; /* C16 info pointer */
  
  sprite = c16_sprite_new_from_file (filename, C16_FILE_BLK);
  
  /* compute the actual width and height.
   * Images are saved as 128x128 blocks in blk */
  width = 128 * c16_sprite_get_blk_width (sprite);
  height = 128 * c16_sprite_get_blk_height (sprite);

  
  image = gimp_image_new (width, height, GIMP_RGB);
  if(image == -1)
    {
      g_print (_("Can't allocate new image\nor file does not exist!\n"));
      gimp_quit();
    }
  
  gimp_image_set_filename (image, filename);
  layer = gimp_layer_new (image, _("Background"), width, height, GIMP_RGB_IMAGE,
                          100, GIMP_NORMAL_MODE);
  gimp_image_add_layer (image, layer, -1);

  drawable = gimp_drawable_get (layer);
  gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, drawable->width,
                       drawable->height, TRUE, FALSE);


  pixel = c16_sprite_get_blk_image (sprite);

  /* draw the image */
  for(i=0; i<height; i++)
    {
      gimp_pixel_rgn_set_rect (&pixel_rgn, pixel[i], 0, i, drawable->width, 1);
      free (pixel[i]);
    }
  free (pixel);
  c16_sprite_free (sprite);

  /* update display */
  gimp_drawable_flush (drawable);
  gimp_drawable_detach (drawable);

  return image;
}

static gint
save_blkimage (char* filename,
               gint32 image_ID,
               gint32 drawable_ID)
{
  GimpDrawable *drawable;
  GimpPixelRgn pixel_rgn;
  guchar **pixels;
  C16Sprite_p sprite;
  C16Image_p *pic;
  int i, k, j, type, blkh, blkw;
  guchar **ptr;
  div_t euklid;
  GimpExportReturnType export;
  gint success;
  
  export = GIMP_EXPORT_EXPORT;
  type = gimp_drawable_type (drawable_ID);
  success = 0;
  
  /* export non-rgb, alpha and multiple layer images */
  if(type != GIMP_RGB_IMAGE)
    {
      gimp_ui_init ("creatures-sprites", FALSE);
      export = gimp_export_image (&image_ID, &drawable_ID, "BLK",
                                  GIMP_EXPORT_CAN_HANDLE_RGB);
    }
  
  /* We really cannot handle other types than rgb
   * therefore only accept (exported) rgb images */ 
  if( export == GIMP_EXPORT_EXPORT)
    {
      drawable = gimp_drawable_get (drawable_ID);
      
      gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, drawable->width,
                           drawable->height, FALSE, FALSE);

      /* height of image counted in blocks of size BLK_BLOCK_SIZE */
      euklid = div (drawable->height, BLK_BLOCK_SIZE);
      blkh = euklid.rem ? euklid.quot+1 : euklid.quot ;

      /* width of image counted in blocks of size BLK_BLOCK_SIZE */
      euklid = div (drawable->width, BLK_BLOCK_SIZE);
      blkw = euklid.rem ? euklid.quot +1 : euklid.quot;

      /* pixels holds the colordata of the whole image including the
       * black stripes that fill the image that height and width are
       * divided by BLK_BLOCK_SIZE */
      pixels = (guchar **)malloc (blkh * BLK_BLOCK_SIZE * sizeof(guchar *));
      for(i=0; i<blkh*BLK_BLOCK_SIZE; i++)
        pixels[i]=(guchar *)calloc (blkw * 3 * BLK_BLOCK_SIZE, sizeof(guchar));

      /* read the colordata and put it into pixels */
      for(i=0; i<drawable->height; i++)
        {
          gimp_pixel_rgn_get_rect (&pixel_rgn, pixels[i], 0, i,
                                   drawable->width, 1);
        }

      /* pic holds the blocks of which a blk acutally consists */
      pic = (C16Image_p *)malloc (sizeof(C16Image_p) * blkh * blkw);

      ptr = (guchar **)malloc (sizeof(guchar *)*BLK_BLOCK_SIZE);
      for(i=0; i<BLK_BLOCK_SIZE; i++)
        ptr[i] = (guchar *)malloc (sizeof(guchar)*3*BLK_BLOCK_SIZE);

      /* bring the blocks in an order that fits the blk format */
      for(j=0; j < blkw; j++)
        {
          for(i=0; i < blkh; i++)
            {
              for(k=0; k < BLK_BLOCK_SIZE; k++)
                {
                  memmove (ptr[k],
                           pixels[i * BLK_BLOCK_SIZE + k] + j * BLK_BLOCK_SIZE * 3,
                           sizeof(guchar) * 3 * BLK_BLOCK_SIZE);
                }
              pic[i + j * blkh] = c16_image_new_with_rgb (BLK_BLOCK_SIZE, BLK_BLOCK_SIZE,
                                                          ptr);
            }
        }
      /* free memory of everything that we no longer need */
      for(i=0; i<drawable->height; i++)
        free (pixels[i]);
      for(i=0; i<BLK_BLOCK_SIZE; i++)
        free (ptr[i]);
      free (pixels);
      free (ptr);

      /* write the blkfile to disk */
      sprite = c16_sprite_new_with_data (S16_TYPE_565_FLAGS, blkh * blkw,
                                     C16_FILE_BLK, blkw, blkh, NULL, pic);
      c16_sprite_compute_headers (sprite);
      c16_sprite_write_to_file (sprite, filename);
      if (c16_status_ok())
        success=1;
      c16_sprite_free (sprite);

      /* delete the created image from export if necessary */
      if(type != GIMP_RGB_IMAGE)
        gimp_image_delete (image_ID); 
    }

  if( export == GIMP_EXPORT_CANCEL)
    success = 2;
  
  return success; 
}
