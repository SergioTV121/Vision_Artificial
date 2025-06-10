//Examen Tinoco Videgaray Sergio Ernesto
//Vision Artificial 5BV1

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

const double e = 2.71828182845904523536;
const double pi = 3.14159265358979311600;
double c = 1 / (2 * pi);

/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
/////////////////////////////////////////////////////////////////////////////

//+++++Kernel Gaussiano+++++
double** crearKernel(int N, double sigma)
{
	double** kernel = new double* [N];	//Reservo memoria para el kernel
	int m = N / 2;	//Mitad del tamano del kernel
	c /= (sigma * sigma);

	for (int i = 0;i < N;i++)
	{
		kernel[i] = new double[N];	//Se reserva memoria en la i-esima posicion del kernel
		for (int j = 0;j < N;j++)
		{
			double x = j - m;
			double y = (i - m) * -1;
			kernel[i][j] = c * pow(e, (-((x * x) + (y * y)) / (2 * (sigma * sigma))));	//Aplico la funcion Gaussiana
		}
	}

	return kernel;
}

Mat aplicarBordes(Mat imagen, int N, int borde)
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

int aplicarConvolucion(Mat imagenBordes, double** kernel, int borde, int x, int y)
{
	int b = (2 * borde) + 1;
	double valor = 0;

	//Se recorre el kernel
	for (int i = 0; i < b; i++) {
		for (int j = 0; j < b; j++) {
			//Se aplica la suma de productos
			valor += double((imagenBordes.at<uchar>(Point(y - borde + i, x - borde + j))) * (kernel[i][j]));
		}
	}

	return valor;
}

Mat aplicarFiltroGaussiano(Mat imagenBordes, double** kernel, int borde)
{
	int rows = imagenBordes.rows;	//Numero de filas
	int cols = imagenBordes.cols; //Numero de columnas
	Mat imagenR = Mat(rows, cols, CV_8UC1, 0.0);	//Imagen Resultante


	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			//Si el valor del pixel es diferente de 0 se aplica la convolucion
			if (imagenBordes.at<uchar>(Point(i, j)) != uchar(0))
			{
				imagenR.at<uchar>(Point(i, j)) = uchar(aplicarConvolucion(imagenBordes, kernel, borde, j, i));
			}
		}
	}

	return imagenR;
}

void imprimirKernel(double** kernel, int N)
{

	cout << "Kernel generado: \n\n";
	for (int i = 0;i < N;i++)
	{
		for (int j = 0;j < N;j++)
		{
			cout << kernel[i][j] << "\t";
		}
		cout << "\n";
	}

}

Mat ecualizarImagen(Mat imagen)
{

	int filas = imagen.rows, columnas = imagen.cols;
	Mat imagenEq(filas, columnas, CV_8UC1);
	double* hist = new double[256]();
	double* histNorm = new double[256]();
	double* histAcum = new double[256]();
	double* histEq = new double[256]();
	int k;
	int num_pixeles = filas * columnas;

	for (int i = 0;i < filas;i++)
	{
		for (int j = 0;j < columnas;j++)
		{
			k = imagen.at<uchar>(Point(i, j));		//Obtengo el histograma
			hist[k]++;
		}
	}


	for (int i = 0;i < 256;i++)
	{
		histNorm[i] = hist[i] / num_pixeles;		//Normalizo el histograma
	}

	histAcum[0] = histNorm[0];

	for (int i = 1;i < 256;i++)
	{
		histAcum[i] = histAcum[i - 1] + histNorm[i];		//Obtengo el histograma acumulado
	}


	for (int i = 0;i < 256;i++)
	{
		histEq[i] = histAcum[i] * 256;		//Reescalo los valores
	}


	for (int i = 0;i < filas;i++)
	{
		for (int j = 0;j < columnas;j++)
		{
			k = imagen.at<uchar>(Point(i, j));
			imagenEq.at<uchar>(Point(i, j)) = uchar(histEq[k + 1]);		//Asigno el valor
		}
	}

	return imagenEq;
}

double convolucionSobel(Mat imagen, int kernel[3][3], int x, int y)
{
	double valor = 0;

	//Se recorre el kernel
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			//Se aplica la suma de productos
			valor += int(imagen.at<uchar>(Point(y - 1 + i, x - 1 + j)) * (kernel[i][j]));
		}
	}

	return valor;
}

Mat aplicarFiltroSobel(Mat imagen) {
	int rows = imagen.rows;	//Numero de filas
	int cols = imagen.cols; //Numero de columnas
	double sx, sy;
	Mat g = Mat(rows, cols, CV_8UC1, 0.0);	//Gradiente 

	int gx[3][3] = { {-1,0,1},{-2,0,2,},{-1,0,1} };		//Operador Gx
	int gy[3][3] = { {1,2,1},{0,0,0,},{-1,-2,-1} };		//Operador Gy


	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//Si el valor del pixel es diferente de 0 se aplica la convolucion
			if (imagen.at<uchar>(Point(i, j)) != uchar(0))
			{
				sx = double(convolucionSobel(imagen, gx, j, i));
				sy = double(convolucionSobel(imagen, gy, j, i));
				g.at<uchar>(Point(i, j)) = uchar(sqrt((sx * sx) + (sy * sy)));	//Calcula la magnitud
			}
		}
	}

	return g;
}

Mat calcularDireccionGradiente(Mat imagen) {
	int rows = imagen.rows;	//Numero de filas
	int cols = imagen.cols; //Numero de columnas
	double sx, sy, angulo;
	Mat g = Mat(rows, cols, CV_8UC1, 0.0);	//Gradiente 

	int gx[3][3] = { {-1,0,1},{-2,0,2,},{-1,0,1} };//Operador Gx
	int gy[3][3] = { {1,2,1},{0,0,0,},{-1,-2,-1} };//Operador Gy


	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//Si el valor del pixel es diferente de 0 se aplica la convolucion
			if (imagen.at<uchar>(Point(i, j)) != uchar(0))
			{
				sx = double(convolucionSobel(imagen, gx, j, i));
				sy = double(convolucionSobel(imagen, gy, j, i));
				angulo = uchar(atan(sy / sx));	//Aplica el arco tangente del gradiente y/x
				g.at<uchar>(Point(i, j)) = uchar((angulo * 180) / pi);
			}
		}
	}

	return g;
}

Mat supresionMaximos(Mat magnitud, Mat direccion, int rows, int cols, int borde) {
	int angulo = 0;
	Mat r = Mat(rows, cols, CV_8UC1, 0.0);	//Matriz resultante

	for (int i = borde;i < rows - borde;i++) {
		for (int j = borde;j < cols - borde;j++) {
			//Acotar el angulo en 4 direcciones (horizontal, vertical y ambas diagonales)
			if (direccion.at<uchar>(Point(i, j)) > 0 && direccion.at<uchar>(Point(i, j)) <= 23)
				angulo = 0;
			else if (direccion.at<uchar>(Point(i, j)) > 23 && direccion.at<uchar>(Point(i, j)) <= 68)
				angulo = 45;
			else if (direccion.at<uchar>(Point(i, j)) > 68 && direccion.at<uchar>(Point(i, j)) <= 113)
				angulo = 90;
			else
				angulo = 135;

			//Evaluar si la magnitud del pixel es mayor a la de sus vecinos en la direccion del gradiente
			//En caso de ser asi, se va a asignar esa misma magnitud
			//En caso contrario se trunca a 0
			switch (angulo) {
			case 0:
				if (magnitud.at<uchar>(Point(i, j - 1)) < magnitud.at<uchar>(Point(i, j)) && magnitud.at<uchar>(Point(i, j)) > magnitud.at<uchar>(Point(i, j + 1)))
					r.at<uchar>(Point(i, j)) = uchar(magnitud.at<uchar>(Point(i, j)));
				else
					r.at<uchar>(Point(i, j)) = uchar(0);
				break;
			case 45:
				if (magnitud.at<uchar>(Point(i - 1, j + 1)) < magnitud.at<uchar>(Point(i, j)) && magnitud.at<uchar>(Point(i, j)) > magnitud.at<uchar>(Point(i + 1, j - 1)))
					r.at<uchar>(Point(i, j)) = uchar(magnitud.at<uchar>(Point(i, j)));
				else
					r.at<uchar>(Point(i, j)) = uchar(0);
				break;
			case 90:
				if (magnitud.at<uchar>(Point(i - 1, j)) < magnitud.at<uchar>(Point(i, j)) && magnitud.at<uchar>(Point(i, j)) > magnitud.at<uchar>(Point(i + 1, j)))
					r.at<uchar>(Point(i, j)) = uchar(magnitud.at<uchar>(Point(i, j)));
				else
					r.at<uchar>(Point(i, j)) = uchar(0);
				break;
			case 135:
				if (magnitud.at<uchar>(Point(i - 1, j - 1)) < magnitud.at<uchar>(Point(i, j)) && magnitud.at<uchar>(Point(i, j)) > magnitud.at<uchar>(Point(i + 1, j + 1)))
					r.at<uchar>(Point(i, j)) = uchar(magnitud.at<uchar>(Point(i, j)));
				else
					r.at<uchar>(Point(i, j)) = uchar(0);
				break;
			}

		}
	}
	return r;
}

Mat histeresis(Mat imagen, int hum_menor, int hum_mayor, int borde) {
	int romper = 0, filas = imagen.rows, col = imagen.cols;
	Mat imagenR = Mat(filas, col, CV_8UC1, 0.0);	//Matriz resultante


	//Recorrer la matriz de direcciones
	for (int i = borde;i < filas - borde;i++) {
		for (int j = borde;j < col - borde;j++) {
		//Si la magnitud esta entre el intervalo del umbral inferior y superior 
		//Se recorre una submatriz buscando algun elemento que supere al umbral mayor para truncar la magnitud a 255
		//En caso de que solamente sea mayor al umbral menor se truncra a 255 directamente
			if (imagen.at<uchar>(Point(i, j)) > hum_menor && imagen.at<uchar>(Point(i, j)) < hum_mayor)
			{
				for (int k = i - borde;k <= i + borde;k++) {
					for (int l = j - borde;l <= j + borde;l++) {
						if (imagen.at<uchar>(Point(k, l)) > hum_mayor) {
							imagenR.at<uchar>(Point(i, j)) = uchar(255);
							romper = 1;		//La bandera cambia su valor puesto que ya se asigno el valor
							break;
						}
					}
					if (romper == 1)
						break;	//Se rompe el ciclo for de k puesto que ya se asigno el valor
				}
			}
			else if (imagen.at<uchar>(Point(i, j)) > hum_menor)
				imagenR.at<uchar>(Point(i, j)) = uchar(255);
		}
	}
	return imagenR;
}


////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "lena.png";
	int N = 5, borde;
	double sigma;
	Mat imagenOriginal, imagenGrises, imagenSuavizada, imagenEcualizada,
		magnitudGradiente, direccionGradiente, imagenBordes, supresionMax;
	/************************/

	/*********Lectura de la imagen*********/
	imagenOriginal = imread(NombreImagen);

	if (imagenOriginal.empty())
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		return -1;
	}
	/************************/

	/************Procesos*********/
	
	cout << "Ingrese el valor de N\n";
	cin >> N;

	while (N%2==0 || N<=1)
	{
		cout << "Por favor ingresa un valor impar para N mayor que 1\n";
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


	double** kernel = crearKernel(N, sigma);	//Creacion del Kernel Gaussiano

	imagenGrises = aplicarBordes(imagenOriginal, N, borde);	//Genera bordes
	imagenSuavizada = aplicarFiltroGaussiano(imagenGrises, kernel, borde);	//Aplica filtro gaussiano
	imagenEcualizada = ecualizarImagen(imagenGrises);		//Ecualiza imagen
	magnitudGradiente = aplicarFiltroSobel(imagenSuavizada);	//Calcula Magnitud del gradiente
	direccionGradiente = calcularDireccionGradiente(imagenSuavizada);//Calcula direccion del gradiente
	supresionMax = supresionMaximos(magnitudGradiente, direccionGradiente, magnitudGradiente.rows, magnitudGradiente.cols, borde);	//Calcula direccion del gradiente con la supresion
	imagenBordes = histeresis(supresionMax, 80, 100, borde);	//Aplica histeresis


	imshow("Imagen Original", imagenOriginal);
	imshow("Imagen en escala de grises", imagenGrises);
	imshow("Imagen suavizada", imagenSuavizada);
	imshow("Imagen ecualizada", imagenEcualizada);
	imshow("Magnitud gradiente", magnitudGradiente);
	imshow("Direccion gradiente", supresionMax);
	imshow("Imagen de bordes", imagenBordes);



	cout << "Tamanio Imagen original: " << imagenOriginal.rows << "x" << imagenOriginal.cols << "pixeles\n";
	cout << "Tamanio Imagen en escala de grises: " << imagenGrises.rows << "x" << imagenGrises.cols << "pixeles\n";
	cout << "Tamanio Imagen suavizada: " << imagenSuavizada.rows << "x" << imagenSuavizada.cols << "pixeles\n";
	cout << "Tamanio Imagen ecualizada: " << imagenEcualizada.rows << "x" << imagenEcualizada.cols << "pixeles\n";
	cout << "Tamanio magnitud gradiente: " << magnitudGradiente.rows << "x" << magnitudGradiente.cols << "pixeles\n";
	cout << "Tamanio direccion gradiente: " << supresionMax.rows << "x" << supresionMax.cols << "pixeles\n";
	cout << "Tamanio Imagen con bordes: " << imagenBordes.rows << "x" << imagenBordes.cols << "pixeles\n";
	
	imprimirKernel(kernel, N);

	/************************/

	waitKey(0); //Funcion para esperar
	return 0;
}