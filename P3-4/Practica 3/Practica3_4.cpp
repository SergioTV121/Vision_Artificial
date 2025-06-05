#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

const double e = 2.71828182845904523536;
const double pi =3.14159265358979311600;
double c = 1 / (2 * pi);

/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
/////////////////////////////////////////////////////////////////////////////

//+++++Kernel Gaussiano+++++
double** crearKernel(int N, double sigma)
{
	double** kernel = new double*[N];	//Reservo memoria para el kernel
	int m = N/ 2;	//Mitad del tamano del kernel
	c /= (sigma * sigma);

	for (int i = 0;i < N;i++)
	{
		kernel[i] = new double[N];	//Se reserva memoria en la i-esima posicion del kernel
		for (int j = 0;j < N;j++)
		{			
			double x = j - m;
			double y = (i - m) * -1;
			kernel[i][j]= c * pow(e, (-((x * x) + (y * y)) / (2* (sigma * sigma))));	//Aplico la funcion Gaussiana
			//cout << kernel[i][j] << "\t";
		}
		//cout << "\n";
	}

	return kernel;
}


Mat aplicarBordes(Mat imagen, int N,int borde) 
{

	int rows = imagen.rows;	//Numero de filas
	int cols = imagen.cols; //Numero de columnas

	Mat imagenG(rows + borde, cols + borde, CV_8UC1);	//Objeto Mat para la imagen en niveles de gris
	Mat imagenB(rows + borde, cols + borde, CV_8UC1);	//Objeto Mat para la imagen resultante con bordes

	cvtColor(imagen, imagenG, COLOR_BGR2GRAY);		//Obtiene la matriz de niveles de gris de la imagen

	double niv_gris;

	for (int i = 0; i < rows + borde; i++) {
		for (int j = 0; j < cols + borde; j++) {

			if ((i >= rows || i < borde) || (j >= cols || j < borde)) {		//En caso de que i o j supere el numero de filas o sea inferior al borde
				imagenB.at<uchar>(Point(i, j)) = uchar(0);	//Se coloca un nivel 0 en la i,j-esima posicion de la matriz resultante
			}
			else {	//En caso contrario

				niv_gris = imagenG.at<uchar>(Point(j - borde, i - borde));
				imagenB.at<uchar>(Point(j, i)) = uchar(niv_gris);	//Coloca el nivel de gris correspondiente a la imagen original
			}

		}
	}

	return imagenB;
}

int aplicarConvolucion(Mat imagenBordes, double** kernel,int borde, int x, int y)
{
	int b = (2 * borde) + 1;
	double valor = 0;

//Se recorre el kernel
	for (int i = 0; i < b; i++) {
		for (int j = 0; j < b; j++) {	
			//Se aplica la suma de productos
			valor += double((imagenBordes.at<uchar>(Point(y - borde + i, x - borde + j)))* (kernel[i][j]));
		}
	}

	return valor;
}

Mat aplicarFiltro(Mat imagenBordes, double** kernel,int borde)
{
	int rows = imagenBordes.rows;	//Numero de filas
	int cols = imagenBordes.cols; //Numero de columnas
	Mat imagenR(rows, cols , CV_8UC1);	//Imagen Resultante


	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols ; j++) {
			//Si el valor del pixel es diferente de 0 se aplica la convolucion
			if (imagenBordes.at<uchar>(Point(i, j)) != uchar(0))
			{
				imagenR.at<uchar>(Point(i, j)) = uchar(aplicarConvolucion(imagenBordes,kernel,borde,j,i));
			}
		}
	}

	return imagenR;
}


////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "lena.png";
	int N=0,borde;
	double sigma;
	Mat imagen, imagenBordes,imagenResultante;
	/************************/

	/*********Lectura de la imagen*********/
	imagen = imread(NombreImagen);

	if (imagen.empty())
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		return -1;
	}
	/************************/

	/************Procesos*********/

	cout << "Ingrese el valor de N\n";
	cin >> N;

	while (N%2==0)
	{
		cout << "Por favor ingresa un valor impar para N\n";
		cin >> N;
	}
	


	cout << "Ingrese el valor de Sigma\n";
	cin >> sigma;

	while (sigma == 0)
	{
		cout << "Por favor ingresa un valor distinto de 0 para sigma\n";
		cin >> sigma;
	}

	borde = N / 2;

	double **kernel=crearKernel(N,sigma);	//Creo el Kernel Gaussiano

		
	imagenBordes = aplicarBordes(imagen,N,borde);	//Genera bordes
	imagenResultante = aplicarFiltro(imagenBordes, kernel, borde);	//Aplica filtro

	
	imshow("Imagen Original", imagen);
	imshow("Imagen con bordes", imagenBordes);
	imshow("Imagen resultante", imagenResultante);

	/************************/

	waitKey(0); //Funcion para esperar
	return 0;
}