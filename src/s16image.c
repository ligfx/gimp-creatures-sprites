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
static gint32 load_s16image (char *);
static gint save_s16image (char *, gint32);


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
  static GimpParamDef loads16_args[] =
    {
      { GIMP_PDB_INT32,    "run_mode",   "Interactive, non-interactive" },
      { GIMP_PDB_STRING,   "filename",    "The name of the file to load" },
      { GIMP_PDB_STRING,   "raw_filename",  "The name of the file to load" }
    };
  
  static GimpParamDef saves16_args[] =
    {
      { GIMP_PDB_INT32, "run_mode", "Interactive, non-interactive" },
      { GIMP_PDB_IMAGE, "image", "Input image" },
      { GIMP_PDB_DRAWABLE, "drawable", "Drawable to save" },
      { GIMP_PDB_STRING, "filename", "The name of the file to save the image" },
      { GIMP_PDB_STRING, "raw_filename", "The name of the file to save the image" },
      { GIMP_PDB_INT32, "type555", "Currently not supported" }
    };
  
  /* return values */
  static GimpParamDef loads16_return_vals[]=
    {
      { GIMP_PDB_IMAGE, "image", "Output image" }
    };
  
  /* count of the input args */
  static gint nloads16_args = sizeof (loads16_args) / sizeof (loads16_args[0]);

  static gint nsaves16_args = sizeof (saves16_args) / sizeof (saves16_args[0]);
  
  /* count of the return values */
  static int nloads16_return_vals = sizeof(loads16_return_vals) /
                                    sizeof(loads16_return_vals[0]);
  
  /* register help and locales */
  gimp_plugin_domain_register (PLUGIN_NAME, LOCALEDIR);
  gimp_plugin_help_register (DATADIR"/help");

  /* register the procedures in the database */
  gimp_install_procedure ("file_s16_load",
                          _("Loads files in s16 image format"),
                          _("This plug-in loads Creatures s16image files"),
                          "Tina Hirsch",
                          "Tina Hirsch",
                          "2001",
                          "<Load>/S16",
                          NULL,
                          GIMP_PLUGIN,
                          nloads16_args,
                          nloads16_return_vals,
                          loads16_args,
                          loads16_return_vals);

  gimp_install_procedure ("file_s16_save",
                          _("Saves files in s16 image format"),
                          _("This plug-in saves Creatures s16image files"),
                          "Tina Hirsch",
                          "Tina Hirsch",
                          "2001",
                          "<Save>/S16",
                          "RGB",
                          GIMP_PLUGIN,
                          nsaves16_args,
                          0,
                          saves16_args,
                          NULL);
  
  /* register the handlers for saving and loading images */
  gimp_register_load_handler ("file_s16_load", "s16", "");
  gimp_register_save_handler ("file_s16_save", "s16", "");
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
  gint success;
  
  success = 0;
  
  values = g_new (GimpParam, 2);
  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = GIMP_PDB_SUCCESS;

  *return_vals  = values;
  run_mode = param[0].data.d_int32; 


  /* Load a s16image.... */
  if (strcmp (name, "file_s16_load") == 0)
    {
      *nreturn_vals = 2;

      image_ID = load_s16image (param[1].data.d_string);

      if( image_ID != -1)
        {
          values[1].type = GIMP_PDB_IMAGE;
          values[1].data.d_image = image_ID;
        }
      else
        values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
    }
  /* ... or save a s16image? */
  else if(strcmp (name, "file_s16_save") == 0)
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
            gimp_get_data ("file_s16_save", &savevals); 
            break; 

          case GIMP_RUN_WITH_LAST_VALS: 
            /*  Possibly retrieve data  */ 
            gimp_get_data ("file_s16_save" , &savevals); 
        }
      
      if(values[0].data.d_status == GIMP_PDB_SUCCESS)
        {
          success = save_s16image (param[3].data.d_string, param[1].data.d_int32);
          if(success == 1)
            gimp_set_data ("file_s16_save", &savevals, sizeof(savevals));
          else if (success ==2)
            values[0].data.d_status = GIMP_PDB_CANCEL;
          else
            values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
        }
    }
  else
    values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
}



static gint load_s16image (char *filename)
{
  gint32 width, height;
  gint32 i, j, count;
  gdouble percentagesteps;
  gint32 image;
  gint32 layer;
  GimpDrawable *drawable;
  GimpPixelRgn pixel_rgn;
  guchar **pixel;  /*t he pixel data */
  C16Sprite_p sprite; /* C16 info pointer  */
  C16Image_p *imagearray;
  gchar name[256];
  gchar *basename, *ptr;
  gint32 *imageswidth, *imagesheight;
  
  sprite = c16_sprite_new_from_file (filename, C16_FILE_S16);
  
  width = 0;
  height = 0;
  count = c16_sprite_get_count (sprite);
  imagearray = c16_sprite_get_images (sprite);
  imageswidth = g_new (guint32, count);
  imagesheight = g_new (guint32, count);
  
  for(i=0; i<count; i++)
    {
      imageswidth[i] = c16_image_get_width (imagearray[i]);
      imagesheight[i] = c16_image_get_height (imagearray[i]);
      if(imageswidth[i] > width)
        width = imageswidth[i];
      if(imagesheight[i] > height)
        height = imagesheight[i];
    }
  
  image = gimp_image_new (width, height, GIMP_RGB);                     

  if(image == -1)                              
    gimp_quit();

  gimp_image_set_filename (image, filename);                                     

  
  basename = strrchr (filename, G_DIR_SEPARATOR);
  if (basename == NULL)
    basename = filename;
  else
    basename++;

  snprintf (name, 256, _("Loading %s, please wait..."), basename);
  gimp_progress_init ( name );
  
  ptr = strrchr (basename, '.');
  if (ptr != NULL)
    *ptr = '\0';
  
  percentagesteps = (gdouble)100/count;

  for(i=0; i<count; i++)
    {
      snprintf (name, 256, "%s_%i", basename, i);

      layer = gimp_layer_new (image, name, imageswidth[i], imagesheight[i],
                              GIMP_RGBA_IMAGE, 100, GIMP_NORMAL_MODE);                                                        
      gimp_image_add_layer (image, layer, -1);                                       
      
      drawable = gimp_drawable_get (layer);
      gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, drawable->width,              
                           drawable->height, TRUE, FALSE);
      pixel = c16_image_get_lines_as_rgba (imagearray[i]);                                        
            
      /* draw the image */                                                             
      for(j=0; j<imagesheight[i]; j++)                                                        
        {                                                                              
          gimp_pixel_rgn_set_rect (&pixel_rgn, pixel[j], 0, j, drawable->width, 1);      
          free (pixel[j]);                                                               
        }
      free (pixel);
      if(percentagesteps * (i+1) <=100)
        gimp_progress_update (percentagesteps * (i + 1));
    }

  g_free (imageswidth);
  g_free (imagesheight);
  c16_sprite_free (sprite);
  return image;                                                                  
}                                                                              
                                                                                  
static gint
save_s16image (char* filename,
               gint32 image_ID)
{
  gint32 drawable_ID;
  GimpDrawable *drawable;
  gint32 *layers;
  gint32 nlayers;
  GimpPixelRgn pixel_rgn;
  guchar **pixels, **newpixels;
  C16Sprite_p sprite;
  C16Image_p *pic;
  int i, j, k, type;
  GimpExportReturnType export;
  gint16 success;
  gint bytes;
  
  export = GIMP_EXPORT_EXPORT;
  success = 0;
  type = GIMP_RGB_IMAGE;
  bytes = 3;
  
  layers = gimp_image_get_layers (image_ID, &nlayers);
  for( i=0; i < nlayers; i++)
    {
      if(gimp_drawable_type (layers[i]) != GIMP_RGB_IMAGE)
        {
          type = gimp_drawable_type (layers[i]);
          drawable_ID = i;
          i = nlayers;
        }
    }
  
  pic = g_new (C16Image_p, nlayers);
  
  /* export non-rgb and alpha images */
  if(type != GIMP_RGB_IMAGE
     && type != GIMP_RGBA_IMAGE)
    {
      gimp_ui_init ("creatures-sprites", FALSE);
      export = gimp_export_image (&image_ID, &drawable_ID, "S16",
                                  GIMP_EXPORT_CAN_HANDLE_RGB
                                  |GIMP_EXPORT_CAN_HANDLE_LAYERS
                                  |GIMP_EXPORT_CAN_HANDLE_ALPHA);
      layers = gimp_image_get_layers (image_ID, &nlayers);
    }
  else
    {
      image_ID = gimp_channel_ops_duplicate (image_ID);
      layers = gimp_image_get_layers (image_ID, &nlayers);
    }

  
  /* We really cannot handle other types than rgb
   * therefore only accept (exported) rgb images*/
  if( export == GIMP_EXPORT_EXPORT)
    {
      for ( i=0; i<nlayers; i++)
        {
          drawable = gimp_drawable_get (layers[nlayers - i - 1]);
          bytes = gimp_drawable_bytes (layers[nlayers - i - 1]);
          
          /* pixels holds the colordata of the whole image  */
          pixels = g_new (guchar*, drawable->height);
          for(j=0; j<drawable->height; j++)
            pixels[j]= g_new0 (guchar, drawable->width * bytes);
      
          gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, drawable->width,
                               drawable->height, FALSE, FALSE);
          
          /* read the colordata and put it into pixels*/
          for(j=0; j<drawable->height; j++)
            gimp_pixel_rgn_get_rect (&pixel_rgn, pixels[j], 0, j,
                                     drawable->width, 1);
            
         if(bytes == 4)
            {
              newpixels=g_new (guchar*, drawable->height);
              
              for(j=0; j < drawable->height; j++)
                {
                  newpixels[j]=(guchar *)g_new0 (guchar, drawable->width * 3);
                  for(k=0; k < drawable->width; k++)
                    {
                      if(pixels[j][4*k+3] != 0)
                          memmove (newpixels[j] + 3 * k, pixels[j] + 4 * k,
                                   sizeof(guchar) * 3);
                    }
                  
                  g_free (pixels[j]);
                }
              g_free (pixels);
              pixels=newpixels;
            }
         pic[i] = c16_image_new_with_rgb (drawable->width, drawable->height,
                                          pixels);

         /* free memory of everything that we no longer need */
         for(j=0; j<drawable->height; j++)
           g_free (pixels[j]);
         g_free (pixels);
        }
      
      /* write the s16file to disk */
      sprite = c16_sprite_new_with_data (S16_TYPE_565_FLAGS, nlayers,
                                         C16_FILE_S16, 0, 0, NULL, pic);
      c16_sprite_compute_headers (sprite);
      c16_sprite_write_to_file (sprite, filename);
      if (c16_status_ok())
        success=1;
      c16_sprite_free (sprite);

      /* delete the created image from export if necessary  */
      if(type != GIMP_RGB_IMAGE
         || bytes == 4)
        gimp_image_delete (image_ID); 
    }

  if( export == GIMP_EXPORT_CANCEL)
    success = 2;
  
  return success; 
}
