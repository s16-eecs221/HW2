/* susie*/

#include <iostream>
#include <cstdlib>

#include "render.hh"


#include <mpi.h>

//mandelbrot function
//return iterations
int mandelbrot(double x, double y) {
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

//create 2-D array using malloc
//return pointer of array
int** arrayGenerater(int rows, int columns) {
    int *metadata = (int *)malloc(rows*columns*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(metadata[columns*i]);
    return array;
}


int
main(int argc, char* argv[]) {
    
    //height and width
    int height, width;
    //rank and # processors
    int rank, np;

    //input size
    if (argc == 3) {
        height = atoi (argv[1]);
        width = atoi (argv[2]);
        assert (height > 0 && width > 0);
    } else {
        fprintf (stderr, "usage: %s <height> <width>\n", argv[0]);
        fprintf (stderr, "where <height> and <width> are the dimensions of the image.\n");
        return -1;
    }
    
    //boundary
    double xmin = -2.1;
    double xmax = 0.7;
    double ymin = -1.25;
    double ymax = 1.25;
    double it = (ymax - ymin)/height;
    double jt = (xmax - xmin)/width;
    double x, y;

    //init pointer to 2d array;

    int **result;
    int **temp;

    //MPI init
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &np);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    //create arrays
    result = arrayGenerater(width, height);
    temp = arrayGenerater(width, height);
    
    //open file for write
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    
    //get the # blocks, assume height % np == 0

    //init position of y
    y = rank * it + ymin;
    int i = 0;
    while (i < height/np)
    {
        //inti position of x
        x = xmin;
        int j = 0;
        while (j < width) {
            //get the iteration times
            temp[i][j] = mandelbrot(x, y);
            printf("rank is %d,data at %d %d is %d\n ",rank,i,j,temp[i][j]);
            x += jt;
            j++;
        }
        y += np*it;
        i++;
    }
    //MPI gather
    MPI_Gather(&(temp[0][0]), height/np* width, MPI_INT, &(result[0][0]), height/np * width, MPI_INT, 0, MPI_COMM_WORLD);
    
    //sync
    MPI_Barrier(MPI_COMM_WORLD);
    //write data  
    if (rank == 0) {
	int counter = 0;
        for (int i = 0; i < height/np; ++i) {
	  for (int k = 0; k < height; k+=height/np) {
            for (int j = 0; j < width; ++j) {
                img_view(j, counter) = render(result[i+k][j]/512.0);
                printf("img at %d %d,resultdata at %d %d is %d\n ",counter,j,i,j,result[i+k][j]);
            }
		counter++;
          }
	}
    }
    //sync
   MPI_Barrier(MPI_COMM_WORLD); 
    free(temp);
    free(result);
    if(rank==0)
    {
        gil::png_write_view("mandelbrot_susie.png", const_view(img));
    }
    MPI_Finalize();
    return 0;
}
