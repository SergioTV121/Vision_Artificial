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

	
	
	//+++++++++++RGB a GRIS++++++++++++
	Mat grises_fun(fila_original, columna_original, CV_8UC1);
	Mat grises_prom(fila_original, columna_original, CV_8UC1);
	Mat grises_ntsc(fila_original, columna_original, CV_8UC1);
	double r, g, b, gris_v, gris_p;
	int i, j;


	for (i = 0;i < imagen.rows;i++)
	{
		for (j = 0;j < imagen.cols;j++)
		{
			b = imagen.at<Vec3b>(Point(j, i)).val[0];
			g = imagen.at<Vec3b>(Point(j, i)).val[1];
			r = imagen.at<Vec3b>(Point(j, i)).val[2];

			//PROMEDIO
			gris_p = (r + g + b) / 3;
			grises_prom.at<uchar>(Point(j, i)) = uchar(gris_p);


			//NTSC
			gris_v = (0.299 * r) + (0.5876 * g) + (0.114 * b);
			grises_ntsc.at<uchar>(Point(j, i)) = uchar(gris_v);

		}
	}

	//FUNCION
	cvtColor(imagen, grises_fun, COLOR_BGR2GRAY);


	//namedWindow("Hola Mundo", WINDOW_AUTOSIZE);//Creación de una ventana
	imshow("Imagen RGB", imagen);
	imshow("Imagen Grises Promedio", grises_prom);
	imshow("Imagen Grises Funcion", grises_fun);
	imshow("Imagen Grises NTSC", grises_ntsc);


	waitKey(0); //Función para esperar
	


	//Importar en EXCEL
	FILE* i_orig, * i_esc_grises_prom, * i_esc_grises_fun, * i_esc_grises_val;
	#pragma warning(suppress : 4996)
	i_orig = fopen("Imagen_Original.xls", "w");
	#pragma warning(suppress : 4996)
	i_esc_grises_fun = fopen("Esc_Grises_Funcion.xls", "w");
	#pragma warning(suppress : 4996)
	i_esc_grises_prom = fopen("Esc_Grises_Promedio.xls", "w");

	int ValorASCII_orig, ValorASCII_eg_prom, ValorASCII_eg_val, ValorASCII_eg_fun;
	float Val_orig, Val_eg_fun, Val_eg_prom;

	for (int i = 0;i < fila_original;i++)
	{
		for (int j = 0;j < columna_original;j++)
		{
			Val_orig = imagen.at<uchar>(Point(j, i));
			ValorASCII_orig = static_cast<int>(Val_orig);
			fprintf(i_orig, "%d \t", ValorASCII_orig);

			Val_eg_fun = grises_fun.at<uchar>(Point(j, i));
			ValorASCII_eg_fun = static_cast<int>(Val_eg_fun);
			fprintf(i_esc_grises_fun, "%d \t", ValorASCII_eg_fun);

			Val_eg_prom = grises_prom.at<uchar>(Point(j, i));
			ValorASCII_eg_prom = static_cast<int>(Val_eg_prom);
			fprintf(i_esc_grises_prom, "%d \t", ValorASCII_eg_prom);
		}
		fprintf(i_orig, "\n");
		fprintf(i_esc_grises_fun, "\n");
		fprintf(i_esc_grises_prom, "\n");
	}
	fclose(i_orig);
	fclose(i_esc_grises_fun);
	fclose(i_esc_grises_prom);
	
	return 0;
}