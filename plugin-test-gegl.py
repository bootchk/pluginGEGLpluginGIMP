#!/usr/bin/env python

'''

'''

from gimpfu import *
import Gegl
from time import sleep

def test(image, drawable):
  ''' Derived from gegl/bindings/pygegl/samples/render-test.py'''
  
  print "Testing gegl."
  
  drawable2 = drawable.copy()
  image.add_layer(drawable2, 1)
  
  node    = Gegl.Node()
  source = node.new_child("gegl:gimp-source", drawableID=drawable.ID)  # .ID
  threshold= node.new_child("gegl:threshold", value=0.4)
  #contrast= node.new_child("gegl:brightness-contrast", contrast=1, brightness=1)
  # blur = node.new_child("gegl:box-blur", radius=3)
  #invert = node.new_child("gegl:invert")
  # multiply = node.new_child("gegl:multiply", value=1.1)
  # over    = node.new_child("gegl:over")
  sink = node.new_child("gegl:gimp-sink", drawableID=drawable2.ID)
  
  # connect nodes into an image processing pipeline
  source >> threshold >> sink
  
  # Make sink pull data through pipeline.
  # For this sink, it ends up in GIMP core (main app, not plugin's Drawable)
  # !!! The sink node flushes an altered version of drawable to GIMP core.
  sink.process()
  gimp.displays_flush()
  
  sleep(5)
  # Since no operation properties have changed, is a no op, but does sink.
  sink.process()
  
  sleep(5)
  threshold.set_property("value", 0.1)
  #contrast.set_property("contrast", -2)
  #blur.set_property("radius", 10)
  sink.process()
  
  # buffer = sink.render((0,0,drawable.width,drawable.height), "RGBA u8")
  
  # Don't drawable.flush() here, it would write original data to GIMP core.
  gimp.displays_flush()
  
  print "Done testing gegl."


register(
        "python_fu_test_gegl",
        "test gegl source and sink",
        "TBD",
        "Lloyd Konneker (bootch nc.rr.com)",
        "Copyright 2012 Lloyd Konneker",
        "2012",
        "<Image>/Filters/Test gegl...",
        "RGB*, GRAY*",
        [],
        [],
        test)

if __name__== '__main__' :
  main()



