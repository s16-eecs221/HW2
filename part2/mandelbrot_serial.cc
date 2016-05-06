/**
 *  \file mandelbrot_serial.cc
 *  \brief Lab 2: Mandelbrot set serial code
 */


#include <iostream>
#include <cstdlib>

#include "render.hh"
#include "timer.c"
#include "timer.h"
using namespace std;

#define WIDTH 1000
#define HEIGHT 1000

int
mandelbrot(double x, double y) {
  int maxit = 511;
  double cx = x;
  double cy = y;
  double newx, newy;

  int it = 0;
  for (it = 0; it < maxit && (x*x + y*y) < 4; ++it) {
    newx = x*x - y*y + cx;
    newy = 2*x*y + cy;
    x = newx;
    y = newy;
  }
  return it;
}

int
main(int argc, char* argv[]) {
  double minX = -2.1;
  double maxX = 0.7;
  double minY = -1.25;
  double maxY = 1.25;
  
  int height, width;
  if (argc == 3) {
    height = atoi (argv[1]);
    width = atoi (argv[2]);
    assert (height > 0 && width > 0);
  } else {
    fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
    fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
    return -1;
  }
 stopwatch_init (); 
  struct stopwatch_t* timer = stopwatch_create (); assert (timer);
  stopwatch_start (timer);
  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  int buff[height][width];
  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);

  y = minY;
  for (int i = 0; i < height; ++i) {
    x = minX;
    for (int j = 0; j < width; ++j) {
      buff[i][j] = mandelbrot(x,y);
      img_view(j, i) = render(buff[i][j]/512.0);
     // img_view(j,i) = render(mandelbrot(x,y)/512.0);
      x += jt;
    }
    y += it;
  }
  gil::png_write_view("mandelbrot_1.png", const_view(img));
  long double t_qs = stopwatch_stop (timer);
  printf ("Sequential one is %Lg seconds.\n", t_qs);
  stopwatch_destroy(timer);
}

/* eof */
