#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include "render.hh"

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
main (int argc, char* argv[])
{

  int height = 1000;
  int width = 1000;

  int *buff;
  int *col;
  int np,rank;
  int i,j,k=0;


  double minX = -2.1; 
  double maxX = 0.7; 
  double minY = -1.25; 
  double maxY = 1.25; 
  double it = (maxY - minY)/height; 
  double jt = (maxX - minX)/width; 
  double x,y; 

  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD,&np);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);


  if(rank == 0)
  {

  	buff = (int*)std::malloc(height*width*sizeof(int));
  	col = (int*)std::malloc(height*sizeof(int));
  }

  MPI_Barrier(MPI_COMM_WORLD);

  for(i=rank;i<height;i+=np)
  {
  	x = minX;
  	y = it * i + minY;
  	for(j=0;j<width;++j)
  	{
  		col[k++] = mandelbrot(x,y);
  		x += jt;
  	}
  	MPI_Gather(col,width,MPI_INT,buff,width,MPI_INT,0,MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0)
  {
  	  k=0;
	  gil::rgb8_image_t img(height, width);
	  auto img_view = gil::view(img);

	  for (int i = 0; i < height; ++i) {
	    for (int j = 0; j < width; ++j) {
	      img_view(j, i) = render(buff[k++]/512.0);
	    }
	  }
	  gil::png_write_view("mandelbrot.png", const_view(img));
	}
	free(buff);
	free(col);
	MPI_Finalize();
	return 0;

}

/* eof */
