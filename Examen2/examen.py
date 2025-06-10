#Tinoco Videgaray Sergio Ernesto
#5BV1 - INTELIGENCIA ARTIFICIAL

import numpy as np
import matplotlib.pyplot as plt
import cv2 as cv
from scipy.spatial import distance
from operator import indexOf
import math


def crearGrupos(valores_pixel,k,l,n):
    grupos=[]
    #Asigno los grupos de la primer iteracion
    for i in range(k-1):
        grupos.append(np.append(valores_pixel[(l*i):(l*(i+1)),:],np.full((l,1),i),axis=1))
    grupos.append(np.append(valores_pixel[l*(k-1):,:],np.full(((l+n%k),1),k-1),axis=1))
    return grupos

def calcularCentroides(grupos,matriz,k):
    means=matriz   #Matriz de los centroides
    #Calcular medias (centroides)
    for i in range(k):
        for j in range(3):
            #Promedio de cada columna donde el grupo coincida con el cluster
            means[i][j]=np.mean(grupos[:,j][grupos[:,3]==i]) 
    return means        

def calcularDistancias(valores_pixel,centroides,n,k):
    distancias=np.zeros((len(valores_pixel),k)) #Matriz de distancias de los pixeles a los grupos
    indices=np.zeros((len(valores_pixel),1))    #Arreglo de los indices con la menor distancia
    for i in range(n):
        for j in range(k):
            distancias[i][j]=distance.euclidean(valores_pixel[i],centroides[j])
        indices[i]=indexOf(distancias[i],min(distancias[i]))  #Indice del valor menor
    return indices

def clustering(valores_pixel,k):
    

    n=len(valores_pixel)
    l=(n//k)        #Division entera

    #Crear grupos
    grupos=crearGrupos(valores_pixel,k,l,n)


    #Juntar los grupos en una matriz
    
    grupos=np.vstack(grupos)

    for i in range(5):  #Iterar 5 veces
        #Calculo los centroides
        centroides=calcularCentroides(grupos,np.zeros((k,3)),k)

        #Calcular distancias de pixeles a cada centroide
        nuevosGpos=calcularDistancias(valores_pixel,centroides,n,k)  

        #Reasignar grupos
        for j in range(n):
            grupos[j,3]=nuevosGpos[j]

    #graficarClusters(grupos,centroides,k) 
    #Reasignar los valores al de los centroides
    for i in range(k):
        grupos[:,:-1][grupos[:,3]==i]=centroides[i] 

    return grupos[:,:-1]    

def graficarClusters(img_grupos,centroides,k):
    
    fig=plt.figure(figsize=(8,8))
    ax=fig.add_subplot(projection="3d")

    colores=['blue','red','green','cyan','yellow','orange','black','pink','brown','purple','olive','#AEBF00','#006E9A','#1818CE','#F6D21C','#A83314']
    #Graficar clusters
    for i in range(k):
        ax.scatter(img_grupos[:,0][img_grupos[:,3]==i],img_grupos[:,1][img_grupos[:,3]==i],img_grupos[:,2][img_grupos[:,3]==i],s=0.5,c=colores[i],label="Color %s" %str(i+1))
    ax.scatter(centroides[:,0],centroides[:,1],centroides[:,2],s=100,c="purple",marker="*")    
    plt.title("Clustering de colores")
    plt.xlabel("Canal R")
    plt.ylabel("Canal G")
    ax.set_zlabel("Canal B")
    plt.show()

def K_means(imagen):
    #Obtencion de los canales RGB
    b,g,r=cv.split(imagen)
    b=b.reshape(-1)
    g=g.reshape(-1)
    r=r.reshape(-1)

    k=5    #Numero de clusters
    #Convertir en arreglo de ternas
    valores_pixel=np.array(list(zip(r,g,b)))
    #Aplicar el clustering a los pixeles
    img_segmentada=clustering(valores_pixel,k)
    criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    ret,label,center=cv.kmeans(np.float32(imagen.reshape((-1,3))),k,None,criteria,10,cv.KMEANS_PP_CENTERS)
    center =np.uint8(center)
 

    #Redimensionar imagen segmentada
    res = center[label.flatten()]
    img_segmentada = res.reshape((imagen.shape))
    return img_segmentada

def segmentarImagen(imagen):

    #Aplicar Kmeans
    img_kmeans=K_means(imagen)

    img=cv.cvtColor(img_kmeans,cv.COLOR_BGR2RGB)
     
    gray = cv.cvtColor(img,cv.COLOR_BGR2GRAY)
    ret, thresh = cv.threshold(gray,0,255,cv.THRESH_BINARY_INV+cv.THRESH_OTSU)
    #Eliminar ruido
    kernel = np.ones((3,3),np.uint8)
    opening = cv.morphologyEx(thresh,cv.MORPH_OPEN,kernel, iterations = 2)
    #Remarcar fondo
    sure_bg = cv.dilate(opening,kernel,iterations=4)
    #Remarcar primer plano
    dist_transform = cv.distanceTransform(opening,cv.DIST_L2,5)
    ret, sure_fg = cv.threshold(dist_transform,0.7*dist_transform.max(),255,0)
    # Regiones sin conocer
    sure_fg = np.uint8(sure_fg)
    unknown = cv.subtract(sure_bg,sure_fg)


    ret, markers = cv.connectedComponents(sure_fg)
    markers = markers+1
    markers[unknown==255] = 0

    #Aplicar watershed
    markers = cv.watershed(img,markers)
    img[markers == -1] = [0,0,255]
    img=cv.cvtColor(img,cv.COLOR_RGB2BGR)

    cv.imshow("Imagen Segmentada usando Kmeans",img_kmeans)
    cv.waitKey(0)
    
    cv.imshow("Imagen Segmentada usando Watershed",img)
    cv.waitKey(0)

    #Remarcar bordes
    rows,cols,_=imagen.shape
    imagen_bordes=np.zeros((rows,cols))
    for i in range(rows):
        for j in range(cols):
            if(markers[i][j]==-1):
                if(i==0 or i==rows-1 or j==0 or j==cols-1):
                    markers[i][j]=1
                else:
                    imagen_bordes[i][j]=255

    return imagen_bordes

def trazarJitomate(img,pixel_inicial):
    pixeles=[]
    pixel_actual=pixel_inicial
    pixeles.append(pixel_actual)
    k=1   
    while(1):
        ban=0
        #Recorrer vecinos y verificar que no sean repetidos
        if(img[pixel_actual[0]+k][pixel_actual[1]+k]==255 and not (pixel_actual[0]+k,pixel_actual[1]+k) in pixeles):
            pixeles.append((pixel_actual[0]+k,pixel_actual[1]+k))
            prox_pixel=(pixel_actual[0]+k,pixel_actual[1]+k)
            ban=1
        if(img[pixel_actual[0]-k][pixel_actual[1]-k]==255 and not (pixel_actual[0]-k,pixel_actual[1]-k) in pixeles):
            pixeles.append((pixel_actual[0]-k,pixel_actual[1]-k)) 
            prox_pixel=(pixel_actual[0]-k,pixel_actual[1]-k)
            ban=1
        if(img[pixel_actual[0]+k][pixel_actual[1]-k]==255 and not (pixel_actual[0]+k,pixel_actual[1]-k) in pixeles): 
            pixeles.append((pixel_actual[0]+k,pixel_actual[1]-k)) 
            prox_pixel=(pixel_actual[0]+k,pixel_actual[1]-k)
            ban=1
        if(img[pixel_actual[0]-k][pixel_actual[1]+k]==255 and not (pixel_actual[0]-k,pixel_actual[1]+k) in pixeles):
            pixeles.append((pixel_actual[0]-k,pixel_actual[1]+k)) 
            prox_pixel=(pixel_actual[0]-k,pixel_actual[1]+k)
            ban=1
        if(img[pixel_actual[0]+k][pixel_actual[1]]==255 and not (pixel_actual[0]+k,pixel_actual[1]) in pixeles):
            pixeles.append((pixel_actual[0]+k,pixel_actual[1])) 
            prox_pixel=(pixel_actual[0]+k,pixel_actual[1])
            ban=1
        if(img[pixel_actual[0]-k][pixel_actual[1]]==255 and not (pixel_actual[0]-k,pixel_actual[1]) in pixeles):
            pixeles.append((pixel_actual[0]-k,pixel_actual[1])) 
            prox_pixel=(pixel_actual[0]-k,pixel_actual[1])
            ban=1
        if(img[pixel_actual[0]][pixel_actual[1]+k]==255 and not (pixel_actual[0],pixel_actual[1]+k) in pixeles):
            pixeles.append((pixel_actual[0],pixel_actual[1]+k)) 
            prox_pixel=(pixel_actual[0],pixel_actual[1]+k)
            ban=1
        if(img[pixel_actual[0]][pixel_actual[1]-k]==255 and not (pixel_actual[0],pixel_actual[1]-k) in pixeles):  
            pixeles.append((pixel_actual[0],pixel_actual[1]-k)) 
            prox_pixel=(pixel_actual[0],pixel_actual[1]-k)
            ban=1
        if(ban==1): #Encontro al menos 1 borde
            k=1 
            pixel_actual=prox_pixel
        else:   #Ban == 0 No encontro mas bordes
            if(k<2):
                k+=1
            else:
                break

        

    return pixeles    

def generarJitomates(img):
    rows,cols=img.shape
    jitomates=[]
    k=-1
    #Recorro la imagen
    for i in range(rows):
        for j in range(cols):
            if (img[i][j]==255):  #Si encuentra un borde
                if(k==-1):
                    jitomate=trazarJitomate(img,(i,j))
                    k=0
                    jitomates.append(jitomate)
                    #print(len(jitomate))
                else:
                    b=0
                    for jit in jitomates:
                        if((i,j) not in jit):    #Jitomate nuevo
                            b+=1  
                    if(b==k+1):
                        jitomates.append(trazarJitomate(img,(i,j)))
                        k+=1  
    return jitomates

def generarLineas(jitomates):
    distancias=[]   #Lista de longitudes
    x1,x2,y1,y2=0,0,0,0 #Coordenadas de cada par de puntos
    lineas=[]   #Lista para los segmentos
    alpha=-1    #Angulo
    coordenadasDicc={"x":[],"y":[]} #Diccionario con las coordenadas
    for k in range(len(jitomates)):
        max_distance=5
        for i in range(len(jitomates[k])):
            for j in range(i+1,len(jitomates[k])):
                distance=math.dist(jitomates[k][i],jitomates[k][j])
                if(distance>max_distance):  
                    x1,x2,y1,y2=jitomates[k][i][1],jitomates[k][j][1],jitomates[k][i][0],jitomates[k][j][0]
                    if(x1!=x2):
                        alpha=angulo(np.array([x1,y1]),np.array([x2,y2]))                
                    if(40>=alpha>36 or alpha==0):
                        coordenadasDicc.update({"x":[x1,x2],"y":[y1,y2]}) 
                        max_distance=distance      
        distancias.append(max_distance)                         
        coordenadasCpy=coordenadasDicc.copy()                
        lineas.append(coordenadasCpy) 
    return lineas,distancias    

def angulo(a, b):
    angulo = round(np.degrees(np.arctan((-a[1]+b[1])/(a[0]-b[0]))), 1)
    return angulo

def visualizarResultado(img,lineas,longitudes):
    img=cv.cvtColor(img,cv.COLOR_RGB2BGR)
    fig, ax = plt.subplots()
    plt.gca().invert_yaxis()
    im = ax.imshow(img)
    print("Coordenadas")
    for linea in lineas:
        print("(",linea["x"][0],linea["y"][0],")\t(",linea["x"][1],linea["y"][1],")")
    ax.set_axis_off()
    ax.set_title("Imagen Resultante")
    #Calcular puntos medios
    pm1=[((lineas[1]["x"][0]+lineas[1]["x"][1])/2),((lineas[1]["y"][0]+lineas[1]["y"][1])/2)]
    pm2=[((lineas[3]["x"][0]+lineas[3]["x"][1])/2),((lineas[3]["y"][0]+lineas[3]["y"][1])/2)]

    #Plotear las lineas 1 y 3
    ax.plot(lineas[1]["x"], lineas[1]["y"],linewidth=2, color='y')
    ax.plot(lineas[3]["x"], lineas[3]["y"],linewidth=2, color='y')

    #Mostrar las longitudes de las lineas
    ax.annotate(str(round(longitudes[1],0)), (pm1[0], pm1[1]))
    ax.annotate(str(round(longitudes[3],0)), (pm2[0], pm2[1]))
    plt.show()

            
def main():
    img=cv.imread('jitomates.png')

    img_segmentada=segmentarImagen(img)

    cv.imshow("Imagen segmentada con bordes",img_segmentada)
    cv.waitKey(0)

    jitomates=generarJitomates(img_segmentada)  

    lineas,distancias=generarLineas(jitomates)
    visualizarResultado(img,lineas,distancias)


if __name__ == "__main__":
    main()
