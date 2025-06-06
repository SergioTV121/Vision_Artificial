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


Mat convertirGrises(Mat imagen,int filas,int columnas) 
{

	Mat imagenG(filas, columnas, CV_8UC1);	//Objeto Mat para la imagen en niveles de gris

	cvtColor(imagen, imagenG, COLOR_BGR2GRAY);		//Obtiene la matriz de niveles de gris de la imagen

	return imagenG;
}

int* obtenerHistograma(Mat imagen,int filas, int columnas) 
{
	int k;
	int* histograma = new int[256]();
	
	for (int i = 0;i < filas;i++)
	{
		for (int j = 0;j < columnas;j++)
		{
			k = imagen.at<uchar>(Point(j, i));		//Obtengo el nivel de intensidad
			histograma[k]++;						//Incremento la frecuencia del nivel
		}
	}

	return histograma;
}

int IsoData(Mat imagen, float T, int filas, int columnas)
{
	float g1 = 0,g2=0,m1,m2,dt=3000;
	float T2=T;
	int* hist;
	//Particionar imagen
	hist = obtenerHistograma(imagen, filas, columnas);

	
	while(dt>5)
	{
		
		g1 = 0;
		g2 = 0;
		for (int k = 0;k <= T;k++)
		{
			g1 += hist[k];
		}

		for (int k = T + 1;k < 255;k++)
		{
			g2 += hist[k];
		}
				

		m1 = (g1) / (T + 1);
		m2 = (g2) / (255 - T);

		dt=T2-abs((m1 + m2) / 2)
		T2 = abs((m1 + m2) / 2);

	}
	return T2;
}


float ValorEntrePicos(Mat imagen, int filas, int columnas)
{
	float g1 = 0, g2 = 0, m1, m2;
	float T = 0;
	int* hist;
	int max = -1,d=0,k=0;
	//Particionar imagen
	hist = obtenerHistograma(imagen, filas, columnas);


	for (int i = 0;i < 254;i++)
	{
		d = pow((i - 255), 2) * hist[i];
		if (d > max)
		{
			max = d;		//Actualizo el valor maximo
			k = i;			//Actualizo el nivel del valor mas alto
		}
	}

	T = ((k - 255) / 2) + 255;
	return T;
}


float Otsu(Mat imagen, int filas, int columnas)
{
	float gb = 0, gf = 0,wb=0,wf=0, mb=0, mf=0, ob=0, of=0, Ow=0,Omin=INFINITY;
	float T = 0;
	int* hist;
	int tam=filas*columnas;
	//Particionar imagen
	hist = obtenerHistograma(imagen, filas, columnas);


	for (int i = 0;i < 254;i++)
	{
		//Calculo gb y gf
		T = i;
		gb = 0;gf = 0;
		for (int j = 0;j<=T;j++)
		{
			gb += hist[j];
		}

		for (int j = T+1;j <=255;j++)
		{
			gf += hist[j];
		}

		//Calculo wb y wf 
		wb = 0; wf = 0;
		for (int j = 0;j <= T;j++)
		{
			wb += hist[j];
		}
		wb /= tam;

		for (int j = T + 1;j <= 255;j++)
		{
			wf += hist[j];
		}
		wf /= tam;

		//Calculo mb y mf
		mb = 0; mf = 0;
		for (int j = 0;j <= T;j++)
		{
			mb += j*hist[j];
		}
		mb /= gb;

		for (int j = T + 1;j <= 255;j++)
		{
			mf += j * hist[j];
		}
		mf /= gf;


		//Calculo Ob y Of
		ob = 0; of = 0;
		for (int j = 0;j <= T;j++)
		{
			ob +=pow(j-mb,2)*hist[j];
		}
		ob /= gb;

		for (int j = T+1;j <= 255;j++)
		{
			of += pow(j - mf, 2) * hist[j];
		}
		of /= gf;

		//Varianza Ow
		Ow = (wb * ob) + (wf * of);
		if (Ow < Omin)
		{
			Omin = Ow;
		}
	}

	return Omin;
}


Mat Binarizar(Mat imagen, int filas, int columnas,int umbral)
{

	Mat imagenB(filas, columnas, CV_8UC1);	//Objeto Mat para la imagen en niveles de gris

	for (int i=0;i < filas;i++)
	{
		for (int j=0;j < columnas;j++)
		{
			if (imagen.at<uchar>(Point(j, i)) >= umbral)
			{
				imagenB.at<uchar>(Point(j, i)) = uchar(255);
			}
			else
			{
				imagenB.at<uchar>(Point(j, i)) = uchar(0);
			}
			
		}
	}

	return imagenB;
}

////////////////////////Inicio de la funcion principal///////////////////
int main()
{

	/********Declaracion de variables generales*********/
	char NombreImagen[] = "piezas2.png";
	float T;
	float isodata, vpicos, otsu;
	double sigma;
	Mat imagen, imagenGrises,imagenId,imagenVp,imagenOtsu;
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

	cout << "Ingrese un umbral inicial\n";
	cin >> T;

	while (T>254 || T<=0)
	{
		cout << "Por favor ingresa un entre 1 y 254\n";
		cin >> T;
	}
	
	
	int filas = imagen.rows;	//Numero de filas
	int columnas = imagen.cols; //Numero de columnas
		
	
	imagenGrises = convertirGrises(imagen, filas,columnas);
	
	isodata=IsoData(imagenGrises, T, filas, columnas);
	vpicos =ValorEntrePicos(imagen,filas,columnas);
	otsu = Otsu(imagen, filas, columnas);


	imagenId = Binarizar(imagenGrises, filas, columnas, isodata);
	imagenVp = Binarizar(imagenGrises, filas, columnas, vpicos);
	imagenOtsu = Binarizar(imagenGrises, filas, columnas, otsu);

	
	
	imshow("Imagen Original", imagen);
	imshow("Imagen IsoData", imagenId);
	imshow("Imagen Valor entre Picos", imagenVp);
	imshow("Imagen Otsu", imagenOtsu);

	cout <<"\n\nIsoData: "<< isodata;
	cout << "\n\nValor entre picos: " << vpicos;
	cout << "\n\nOtsu: " << otsu;

	/************************/

	waitKey(0); //Funcion para esperar
	return 0;
}