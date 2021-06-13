'''import pyb, sensor, image, time, math
enable_lens_corr = False # turn on for straighter lines...
sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
   clock.tick()
   img = sensor.snapshot()
   if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

   # `merge_distance` controls the merging of nearby lines. At 0 (the default), no
   # merging is done. At 1, any line 1 pixel away from another is merged... and so
   # on as you increase this value. You may wish to merge lines as line segment
   # detection produces a lot of line segment results.

   # `max_theta_diff` controls the maximum amount of rotation difference between
   # any two lines about to be merged. The default setting allows for 15 degrees.
   is_line = 0
   for l in img.find_line_segments(merge_distance = 30, max_theta_diff = 45):
      if ((is_line == 0) & (40 <= l.x1()) & (l.x1() <= 120) & (40 <= l.x2()) & (l.x2() <= 120) \
      & ((l.y1() <= 5) | (l.y2() <= 5))):
         img.draw_line(l.line(), color = (0, 255, 0))
         is_line = 1
         data = (l.x1(), l.y1(), l.x2(), l.y2())
      else:
         img.draw_line(l.line(), color = (255, 0, 0))

   if is_line == 1:
      uart.write(("y %d %d %d %d\n" % data).encode())
      print("y %d %d %d %d\n" % data)
   else:
      uart.write(("n\n").encode())
      print("n\n")

   print("FPS %f" % clock.fps())
'''




import pyb, sensor, image, time, math

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

def degrees(radians):
   return (180 * radians) / math.pi

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)

while(True):
   clock.tick()
   img = sensor.snapshot()
   for tag in img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y): # defaults to TAG36H11
      img.draw_rectangle(tag.rect(), color = (255, 0, 0))
      img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
      # The conversion is nearly 6.2cm to 1 -> translation
      '''print_args = (tag.id(), tag.x_translation(), tag.y_translation(), tag.z_translation(), \
            degrees(tag.x_rotation()), degrees(tag.y_rotation()), degrees(tag.z_rotation()))'''
      print_args = (tag.x_translation(), tag.z_translation())
      # Translation units are unknown. Rotation units are in degrees.
      if (tag.x_translation() > 0):
        uart.write(("R Tx %.3f, Tz %.3f" % print_args).encode())
        print("R Tx: %.3f, Tz: %.3f\n" % print_args)
      else:
        uart.write(("L Tx %.3f, Tz %.3f" % print_args).encode())
        print("L Tx: %.3f, Tz: %.3f\n" % print_args)
   # uart.write(("FPS %f\r\n" % clock.fps()).encode())
