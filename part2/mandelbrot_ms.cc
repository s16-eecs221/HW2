/**
 *  \file mandelbrot_ms.cc
 *
 *  \brief Implement your parallel mandelbrot set in this file.
 */

#include <iostream>
#include <cstdlib>
 #include <mpi.h>

#include "render.h"
#define WIDTH 1000
#define HEIGHT 1000


//function from professor
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
main (int argc, char* argv[])
{
  /* Lucky you, you get to write MPI code */
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

  double it = (maxY - minY)/height;
  double jt = (maxX - minX)/width;
  double x, y;

  int i,j;
  int rows,cols;
  int rank,np;
  int end;

  int row[WIDTH];
  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&np);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);


  MPI_Status status;

  if (rank == 0)
  {
  	i = 0;
  	while(i < np-1)
  	{
  		MPI_Send(&i,1,MPI_INT,i+1,0,MPI_COMM_WORLD);
  		i++;
  	}

  	while()
  	


  }

  else
  {
  		while(1)
  		{
  			MPI_Recv(&i,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
  			MPI_Send(temp,width,MPI_INT,0,i,MPI_COMM_WORLD);
  		}
  }
  if(rank == 0)
  {
  	gil::png_write_view("mandelbrot.png", const_view(img));
	}
  MPI_Finalize();
}

}

/* eof */
