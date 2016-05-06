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

  int np,rank;
  int i,j,k=0;
  int N;

 double minX = -2.1; 
  double maxX = 0.7; 
  double minY = -1.25; 
  double maxY = 1.25; 
  double it = (maxY - minY)/height; 
  double jt = (maxX - minX)/width; 
  double x,y;
  int* buff;
  int* col; 

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&np);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 // printf("np is %d\n",np);
 // printf("rank is %d\n",rank);
  if(rank == 0)
  {
    total_buff = (int*)std::malloc(height*width*sizeof(int));  
     buff = (int*)std::malloc(height*(height/np)*sizeof(int));  
     col = (int*)std::malloc(height*sizeof(int));
     if(!buff || !col){fprintf(stderr,"Malloc failed\n");exit(1);}
  } 
  gil::rgb8_image_t img(height, width);
  auto img_view = gil::view(img);
  MPI_Barrier(MPI_COMM_WORLD);

  k = 0;
  
  for(y=minY,i=rank;i< height;i+=np,y+=it)
  {
    for(x = minX,j=0;j<width;++j)
    {
      col[k++] = mandelbrot(x,y);
      x += jt;
    }
    MPI_Gather(col,width,MPI_INT,buff,width,MPI_INT,0,MPI_COMM_WORLD);
  }
  MPI_Gather(buff,width*(height/np),MPI_INT,total_buff,width*(height/np),MPI_INT,0,MPI_COMM_WORLD);


  MPI_Barrier(MPI_COMM_WORLD);
 // printf("to write\n");
  if (rank == 0)
  {
    k = 0;

    for (i = 0; i < height,k<height*width; ++i) {
      for (j = 0; j < width,k<height*width; ++j) {
        img_view(j, i) = render(total_buff[k++]/512.0);
      }
    }
    gil::png_write_view("mandelbrot_susie.png", const_view(img));
  }

  MPI_Barrier(MPI_COMM_WORLD);

  free(buff);
  free(col);
  MPI_Finalize();
  return 0;

}