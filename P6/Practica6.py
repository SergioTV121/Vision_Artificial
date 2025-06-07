import numpy as np
import math
import cv2

c=1/(math.pi)
filas=0
columnas=0



def crearKernel(tam,sigma,med):
    kernel=np.zeros((tam,tam))

    for i in range(tam):
        for j in range(tam):
            x=j-med
            y=(i-med)*-1
            p=(x**2)+(y**2)
            l=(c/(sigma**4))*(1-((p)/(2*(sigma**2))))*(math.e**-(p/(2*(sigma**2))))
            kernel[i][j]=l
    return kernel


def generarBordes(imagen,tam,borde):
    rows,cols,_ = imagen.shape
	
    imagenB=np.zeros((rows+borde,cols+borde),dtype=int)
    imagenG=cv2.cvtColor(imagen, cv2.COLOR_BGR2GRAY)       #Obtiene la matriz de niveles de gris de la imagen

    for i in range(rows + borde):
        for j in range(cols + borde):
            if ((i >= rows or i < borde) or (j >= cols or j < borde)):
                imagenB[i][j] = 0	#Se coloca un nivel 0 en la i,j-esima posicion de la matriz resultante
            else:
                niv_gris = imagenG[i-borde][j-borde]    
                imagenB[i][j] = niv_gris #Coloca el nivel de gris correspondiente a la imagen original
			
  
    return imagenB/255.0    #Escalar valores


def aplicarConvolucion(imagen,kernel,x,y,borde):
    b=(2*borde)+1
    v=0
    for i in range(b):
            for j in range(b):
                v+=float(imagen[y-borde+i][x-borde+j]*kernel[i][j])
    return v

def aplicarKernel(imagen,kernel,tam):
    med=tam//2
    dt=0
    filas,columnas=imagen.shape
    imagenR=np.zeros((filas,columnas),dtype=float)
    for i in range(med,filas-med):
        for j in range(med,columnas-med):
            if(imagen[i][j]!=0):
                imagenR[i][j]=float(aplicarConvolucion(imagen,kernel,j,i,med))
    #Binarizar                
    imagenR[imagenR >= 0] = 0
    imagenR[imagenR < 0] = 1    #Cruce por 0 agrega borde

    return imagenR

tam=0
sigma=0
    
while(tam%2==0 or tam<5 ):
    tam=int(input("\nIngrese un tamaño para el Kernel mayor a 3: "))

while(sigma==0):
    sigma=float(input("\nIngrese el valor de sigma: "))

med=tam//2
kernel=crearKernel(tam,sigma,med)

imgOriginal=cv2.imread("lena.png")
imgOriginal = cv2.medianBlur(imgOriginal,5)
imgBordes=generarBordes(imgOriginal,tam,med)
imgResultante = aplicarKernel(imgBordes,kernel,tam)

cv2.imshow("Imagen original",imgOriginal)
cv2.imshow("Imagen con bordes",imgResultante)
cv2.waitKey(0)