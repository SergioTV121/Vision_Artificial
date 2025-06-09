#Almeraya Pineda Kimberly Jovana
#Tinoco Videgaray Sergio Ernesto
"""
Tornillo    tx      ty       angulo
    1       30      60       20
    2       30      180      45
    3       30      230      30
    4       1       1        0
    5       160     450      60
    6       100     500      70
    7       30      100      20
    8       100     500      65
    9       50      500      65
"""
import cv2 
import numpy as np


def draw_circle(event,x,y):
    global vertices,img_cpy
    if event == cv2.EVENT_LBUTTONDBLCLK:
        cv2.circle(img_cpy,(x,y),8,(0,255,0),-1)   #Marcar un circulo verde
        vertices.append((x,y))   #Agregar las coordenadas a la lista

def marcarPuntos(img):
    global vertices

    cv2.namedWindow('image')
    cv2.setMouseCallback('image',draw_circle)

    while(1):   #Bucle infinito
        cv2.imshow('image',img) #Muestra la imagen
        k=cv2.waitKey(20) & 0xFF    #Obtencion de la tecla presionada
        if (k == ord('a') or len(vertices)>=4): 
            break#Itera hasta presionar la tecla 'a' o hasta tener 4 coordenadas

    cv2.destroyAllWindows() #Cierra todas las ventanas del opencv

def transformar(m):
    global tx,ty,angulo
    
    m=np.array(m).reshape((3,1)) #Convertir pixel en vector columna

    h=np.matrix(    #Matriz de rotacion euclideana
        [[np.cos(angulo),-1*np.sin(angulo),  ty],
        [np.sin(angulo),np.cos(angulo),     -tx],
        [0,             0,                  1]]
    )

    h_inv=np.linalg.inv(h)  #Obtener matriz inversa
    m_t=h_inv*m #Multiplicar matriz inversa por el pixel
    m_t=(np.squeeze(m_t)).astype(int)
    return(m_t)


vertices=[]     #Arreglo de las coordenadas
img=cv2.imread("P9/piezas.png") #Imagen original
img_cpy=img.copy()  #Copia de la imagen original

marcarPuntos(img_cpy)

img=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
rows,cols=img.shape
tx,ty=1,1
angulo=0

tx=float(input("Ingrese tx "))
ty=float(input("Ingrese ty "))
angulo=float(input("Ingrese el angulo en grados "))

angulo=np.deg2rad(angulo) #Convertir a radianes
img_t=np.zeros((rows+50,cols+50),dtype=np.float32)

for i in range(vertices[0][1],vertices[2][1]):
    for j in range(vertices[0][0],vertices[1][0]):
        m_t=transformar([i,j,1])
        
        if((m_t>=0).all() and (m_t[0,0]<img_t.shape[0] and m_t[0,1]<img_t.shape[1])):
            img_t[m_t[0,0],m_t[0,1]]=(img[i,j])/255

img_t=cv2.medianBlur(img_t,3)
cv2.imshow("",img_t)
cv2.waitKey(0)