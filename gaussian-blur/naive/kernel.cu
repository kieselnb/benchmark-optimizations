
#include "cuda_runtime.h"
//#include "device_functions.h"
#include "device_launch_parameters.h"
#include "cuda.h"
#include "book.h"
//#include "glut.h"
#include <GL/glut.h>
#include "stdio.h"
#include "iostream"
#include "math.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"

#define DIM 27     //GAUSSIAN FILTER DIMENSIONS MAX == 25 !!!
#define PI 3.1415926535897932385

//SPEEDING UP PURPOSES
__constant__ float matrix[DIM][DIM];

//DECLARED GLOBALLY
unsigned char *image_orig = NULL;
unsigned char *image_Gauss = NULL;
unsigned char *image = NULL;
int Width = NULL;
int Height = NULL;

/*
CREATE GAUSSIAN IMAGE FILTER MATRIX
SINGLE PRECISION IS QUITE ENOUGHT
*/
void fill(float rho,float *field){
	float e = 2.7182818284590452354f;
	for(int i =0;i<DIM;i++){
		for(int j=0;j<DIM;j++){
			int offset = DIM*i+j;
			//printf("   %d   \n",i);
			float x2 = pow((float)(j-(DIM/2)),2.0f);  //X RAISED TO POWER OF 2
			float y2 = pow((float)(i-(DIM/2)),2.0f);  //Y RAISED TO POWERR OF 2
			float rho2 = pow(rho,2.0f);               //RHO RAISED TO POWER OF 2
			float temp1 = -( (x2+y2)  / (2*rho2));    //EXPONENT OF E
			float tempx = pow(e,temp1);               //E RAISED TO POWERED OF TEMP1
			float base = 1/(2*PI*rho2);               //BASE
			float res = base*tempx;                   //RESULT OF GAUSSIAN FILTER FOR X & Y VALUES
			field[offset] = res;
		}
	}
};

//BEGIN OpenGL FUNCTIONS
static void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
  }
}

void myMenu(int menuItemID){

  switch(menuItemID) {
	case 1:
		image = image_orig;
		glutPostRedisplay();
		break;
	case 2:
		image = image_Gauss;  
		glutPostRedisplay();
		break;
    case 3:
      exit(0);
      break;
  }
}


void DrawImage( void ) {
   glClearColor( 0.0, 0.0, 0.0, 1.0 );
   glClear( GL_COLOR_BUFFER_BIT );
  if((image != NULL)) {
	glDrawPixels( Width, Height, GL_RGBA , GL_UNSIGNED_BYTE, image );
  }
  glutSwapBuffers();
 }
//END OpenGL FUNCTIONS


/*
MAIN GPU COMPUTING KERNEL
*/
__global__ void Gaussian(unsigned char *input,unsigned char *output,int *addittional)
{

	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int z = threadIdx.z + blockIdx.z * blockDim.z;
	int offset = x + y * blockDim.x * gridDim.x;
	int bloff = threadIdx.y * blockDim.x + threadIdx.x;
	int tx = threadIdx.x; int ty = threadIdx.y;
	int bdx = blockDim.x; int bdy = blockDim.y;
	int width = addittional[0];
	int height = addittional[1];
	float conv_res = 0;

	
	//MAIN CONVOLUTION PART                                                             //////////////////// NEW //////////////////////////
	if (offset < (4*width * height)){
		int ii = 0;
		int jj = 0;
		
		for(int i = -DIM/2;i<(DIM/2);i++){
			for(int j = -DIM/2;j<(DIM/2);j++){
				int temp = 0; //DEFAULT VALUE WHEN OUT-OF-RANGE
				int new_x = x+j;
				int new_y = i+y;
				if((new_y<=height) && (new_y >= 0) && (new_x >=0 ) && (new_x <= width)){  //OUT-OF-RANGE?
					int new_offset = new_y*width + new_x;
					temp = input[4*new_offset+z];
				}

				conv_res +=temp*matrix[ii][jj];
				jj++;
			}
			jj = 0;
			ii++ ;
		}

		//RENEW USED VARIABLES AND SAVE
		tx = threadIdx.x; ty = threadIdx.y;
		output[4*offset+z] = (unsigned char) round(conv_res);
	}
	                                                                                   //////////////////// NEW //////////////////////////

}

int main(int argc,char **argv)
{
	cudaEvent_t start,stop;
	float elapsedTime = 0;

	//CREATE FILTER MATRIX
	float rho = 8.0;
	float *host_matrix = (float*)malloc(DIM*DIM*sizeof(float));
	//float *dev_matrix;
	fill(rho,host_matrix);

	//IMAGE LOADING
	int chan = 4;
    int width,height,bits;  //x = WIDTH   y = HEIGHT n = BITS/Pix
    unsigned char *host_data = stbi_load("image.png", &width, &height, &bits, chan); //4 MEANS RGBA
	if(host_data == NULL) printf("Loading Failed \n");

	//DISPLAY IMAGE INFO
	Width = width;
	Height = height;
	printf("Image Size: %d Bits \n",width*height*bits);
	printf("WIDTH: %d Height: %d \n",width,height);

	//ALLOCATE
	unsigned char *host_res = (unsigned char*)malloc(width*height*chan*sizeof(char));
	int *host_addittional = (int*)malloc(3*sizeof(int));
	unsigned char *dev_input;
	unsigned char *dev_output;
	//float *dev_matrix;
	int *dev_addittional;
	host_addittional[0] = width;
	host_addittional[1] = height;
	host_addittional[2] = bits;


	//ALLOCATE FOR INPUT
	//HANDLE_ERROR(cudaMalloc((void**)&dev_matrix,DIM*DIM*sizeof(float)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_addittional,3*sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_input,width*height*chan*sizeof(char)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_output,width*height*chan*sizeof(char)));

	//HANDLE_ERROR(cudaMalloc((void**)&dev_matrix,DIM*DIM*sizeof(float)));
	HANDLE_ERROR(cudaEventCreate(&start));
	HANDLE_ERROR(cudaEventCreate(&stop));
	HANDLE_ERROR(cudaEventRecord(start,0));


	//COPY INPUT
	HANDLE_ERROR(cudaMemcpy(dev_input,host_data,chan*width*height*sizeof(char),cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_addittional,host_addittional,3*sizeof(int),cudaMemcpyHostToDevice));
	//HANDLE_ERROR(cudaMemcpy(dev_matrix,host_matrix,DIM*DIM*sizeof(float),cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpyToSymbol(matrix,host_matrix,DIM*DIM*sizeof(float)));


	//DEFINE DIMENSIONS
	//NOTE THAT WE ARE GOING TO LAUNCH 3D GRID (AS EACH Z.DIM for R,G,B,A)
	int th = 1024;
	dim3 BLOCKS_PER_GRID(width/th ,height,chan);
	dim3 THREADS_PER_BLOCK(th,1);

	//LAUNCH KERNEL
	Gaussian<<<BLOCKS_PER_GRID,THREADS_PER_BLOCK>>>(dev_input,dev_output,dev_addittional);

	//COPY BACK RESULTS
	HANDLE_ERROR(cudaMemcpy(host_res,dev_output,chan*width*height*sizeof(char),cudaMemcpyDeviceToHost));

	//GET AND DISPLAY CALCULATION TIME
	HANDLE_ERROR(cudaEventRecord(stop,0));
	HANDLE_ERROR(cudaEventSynchronize(stop));
	HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime,start,stop));
	printf("CUDA Gaussian Image Filter Completed in: %7.4f ms \n",elapsedTime);

	//DISPLAY IMAGE
	image_Gauss = host_res;
	image = host_data;
	image_orig = host_data;
	glutInitWindowSize(width, height);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE |GLUT_RGBA);
    glutCreateWindow("Taylor Swift");
    glutKeyboardFunc(Keyboard);
	glutDisplayFunc(DrawImage);

	glutCreateMenu(myMenu);
	glutAddMenuEntry("Original", 1);
	glutAddMenuEntry("Gaussian", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();

	//CLEAN MEMORY
	free(host_matrix);
	free(host_addittional);
	free(host_res);
	free(host_data);
	//cudaFree(dev_matrix);
	cudaFree(dev_input);
	cudaFree(dev_output);
	cudaFree(dev_addittional);

    return 0;
   
}
