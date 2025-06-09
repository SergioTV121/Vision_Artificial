
import cv2 
import numpy as np
import tkinter
from tkinter import filedialog




def draw_circle(event,x,y,flags,param):
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
        if (k == ord('a') or len(vertices)>=2): 
            break#Itera hasta presionar la tecla 'a' o hasta tener 4 coordenadas

    cv2.destroyAllWindows() #Cierra todas las ventanas del opencv

def calcularMetricas(vert,n,m):
    xn=(vert[0][0]+vert[1][0])/(2*m)
    yn=(vert[0][1]+vert[1][1])/(2*n)

    wn=(vert[1][0]-vert[0][0])/2
    hn=(vert[1][1]-vert[0][1])/2

    return xn,yn,wn,hn

for i in range(2):
    vertices=[]     #Arreglo de las coordenadas
    #Abrir archivo
    tkinter.Tk().withdraw() # prevents an empty tkinter window from appearing
    rutaImg = filedialog.askopenfilename(initialdir='./P9/imagenes/',title = "Selecciona el archivo",filetypes = (("png Files","*.*"),("all files","*.*")))
    img=cv2.imread(rutaImg)
    img_cpy=img.copy()  #Copia de la imagen original

    marcarPuntos(img_cpy)

    img=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    rows,cols=img.shape

    xn,yn,wn,hn=calcularMetricas(vertices,rows,cols)

    print(vertices,rows,cols)
    print(xn,yn,wn,hn)

    file = open("./metricas"+str(i)+".txt","w")
    file.write(str(xn)+" "+str(yn)+" "+str(wn)+" "+str(hn))
    file.close()