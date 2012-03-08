/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */

#include "config.h"
#include <glib/gi18n-lib.h>





#ifdef GEGL_CHANT_PROPERTIES

// Declare property of node, i.e. parameter to operation
// Int ... min, max, default, blurb
gegl_chant_int(drawableID, _("Drawable ID"), 0, 1000000, 0,
		  _("ID of GIMP Drawable to move in GEGL pipeline"))

#else

// Configure gegl-chant.h
/* <<< */      
#define GEGL_CHANT_TYPE_SINK 
#define GEGL_CHANT_C_FILE       "gimp-sink.c"

// This generates boilerplate e.g. structs, set_property(), etc.
#include "gegl-chant.h"

/*
Depends on GIMP libgimp.
If this file is in gegl/operations/external,
modify gegl/configure.ac and gegl/operations/external/Makefile.am
to check presence of GIMP-2.0 directory and specify GIMP_CFLAGS, GIMP_LIBS make variables.
*/
#include <gimp-2.0/libgimp/gimp.h>

/* <<< */      
// No bounding_box(), prepare()

#define GIMP_SINK_SCALE 1.0

// !!! Note GIMP abbreviates Rgn instead of region, and gimp_ prefix to functions
// Compare to plug-ins/pygimp/pygimp-tile.c
static gboolean
process (GeglOperation        *operation,
         GeglBuffer           *data,
         const GeglRectangle  *result)
{
  GeglChantO *o = GEGL_CHANT_PROPERTIES (operation);

  g_printf("Process sink\n");
  if (! o->drawableID)
    return FALSE;
  else
  {
    GimpDrawable            *drawable;
    const Babl              *format;
    GimpPixelRgn             io_pixel_region;
    gpointer                 pr;
    
    g_printf("result width %d\n", result->width);
    
    drawable = gimp_drawable_get(o->drawableID);
    // format = gegl_operation_get_format (operation, "output");
    format = babl_format ("RGBA u8");
    // TODO test format of operation with format of drawable

    gimp_pixel_rgn_init (&io_pixel_region, drawable,
                     result->x,     result->y,
                     result->width, result->height,
                     TRUE, TRUE);

    for (pr = gimp_pixel_rgns_register (1, &io_pixel_region);
         pr;
         pr = gimp_pixel_rgns_process (pr))
      {
        GeglRectangle rect = { io_pixel_region.x, io_pixel_region.y, io_pixel_region.w, io_pixel_region.h };

        gegl_buffer_get (data, GIMP_SINK_SCALE, &rect, format, io_pixel_region.data, io_pixel_region.rowstride);
      }

    /* 
    Drawable is set of tiles private to this sink.
    Not the same as the caller's, nor the same as the source node's.
    */
    gimp_drawable_flush(drawable);
    gimp_drawable_merge_shadow(o->drawableID, FALSE);
    gimp_drawable_update(o->drawableID, result->x,     result->y,
                     result->width, result->height);
    return TRUE;
  }
}


static void
gegl_chant_class_init (GeglChantClass *klass)
{
  GeglOperationClass       *operation_class;
  GeglOperationSinkClass *sink_class;
  
  // cast
  operation_class = GEGL_OPERATION_CLASS (klass);
  sink_class    = GEGL_OPERATION_SINK_CLASS (klass);

  // make object method attributes refer to reimplemented virtual (or default) methods
  sink_class->process = process;
  sink_class->needs_full = FALSE;

  operation_class->name        = "gegl:gimp-sink";
  operation_class->compat_name = "gegl:save-drawable";
  operation_class->categories  = "programming:output";
  operation_class->description =
        _("Sink to a GIMP Drawable, for use by GIMP plug-ins.");

  /* operation_class->no_cache = TRUE; */
}

#endif
