#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////Espacio de nombres////////////////////////////
using namespace cv;
using namespace std;
/////////////////////////////////////////////////////////////////////////////

////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "lena.png";
	Mat imagen; // Matriz que contiene nuestra imagen sin importar el formato
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
	int fila_original = imagen.rows;
	int columna_original = imagen.cols;//Lectura de cuantas columnas

	cout << "filas: " << fila_original << endl;
	cout << "columnas: " << columna_original << endl;

	namedWindow("Hola Mundo", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("Hola Mundo", imagen);

	/************************/

	waitKey(0); //Función para esperar
	return 0;
}