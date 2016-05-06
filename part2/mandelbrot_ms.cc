/* master-slave*/

#include <iostream>
#include <cstdlib>

#include "render.hh"

using namespace std;
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
    //for tag the ms process
    int row;
    int source,tag;
    //define end = -1 as the finish sign.
    int end = -1;

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
    //init status
    MPI_Status status;
    //MPI init
    MPI_Init (&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &np);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    //create arrays
    result = arrayGenerater(width, height);
    int line[width];
  //  printf("line is at %p",line);
    
    //open file for write
    gil::rgb8_image_t img(height, width);
    auto img_view = gil::view(img);
    


//printf("start\n");
    //start
    if(rank==0)
    {
        //master
        //bcast
        for(row=0;row<np-1;row++)
        {    MPI_Send(&row,1,MPI_INT,row+1,0,MPI_COMM_WORLD);
	 //    printf("send %d to %d th slave\n",row,row+1);
	}
       // printf("init sending success\n");
        //receive and send
        for (row=np-1;row<height;row++){
            MPI_Recv(line,width,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	    tag=status.MPI_TAG;
            source=status.MPI_SOURCE;
           // printf("root receive from %d with %d the rows finished\n",source,tag);

            for (int i = 0; i < width; ++i)
            {
                result[tag][i] = line[i];
		//printf("result at %d %d is %d",tag,i,line[i]);
            }	    
           // printf("receive success\n");	
            MPI_Send(&row,1,MPI_INT,source,0,MPI_COMM_WORLD);
         //   printf("root sending to %d\n",source);
        }

        //stop slave from receiving
        for(row=0;row<np-1;row++){
           MPI_Recv(line,width,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            tag=status.MPI_TAG;
            source=status.MPI_SOURCE;
           // printf("receive last gourp of slaves with job %d",tag);    
            for (int i = 0; i < width; ++i)
            {
                result[tag][i] = line[i];
            }
//printf("sending end signal to slave");
           MPI_Send(&end,1,MPI_INT,source,0,MPI_COMM_WORLD);
        }
    }    
    else 
    {
        //slave
    while(1){
        MPI_Recv(&row,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
	//printf("%d slave receive from root",row);
        //nothings to do
        if (row==-1) 
{//printf("all finished!\n");
break;}
	
        x = xmin;
        y = ymin + row * it;
        //calculate
        for(int j=0;j<width;j++){
            line[j]=mandelbrot(x,y);
            x += jt;
        }
//printf("finish cal\n");
        MPI_Send(line,width,MPI_INT,0,row,MPI_COMM_WORLD);
	//printf("sending to root with row equals %d\n",row);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //calculate the final valuei
   // printf("final calculate");
    if(rank == 0)
    {
        for(int i =0;i<height;i++)
        {
            for (int j = 0; j < width; ++j)
            {
                img_view(j,i) = render(result[i][j]/512.0);
		//printf("result at %d %d equals to %d\n",i,j,result[i][j]);
            }
        }
    }
    //printf("finish match!\n");
    MPI_Barrier(MPI_COMM_WORLD);
    free(result);
    if(rank==0)
    {
        gil::png_write_view("mandelbrot_ms.png", const_view(img));
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}
