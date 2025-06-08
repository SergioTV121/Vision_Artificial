
import numpy as np
import matplotlib.pyplot as plt
import cv2
from sklearn.cluster import KMeans
from scipy.spatial import distance
from operator import indexOf


def crearGrupos(valores_pixel,k,l,n):
    grupos=[]
    #Asigno los grupos de la primer iteracion
    for i in range(k-1):
        grupos.append(np.append(valores_pixel[(l*i):(l*(i+1)),:],np.full((l,1),i),axis=1))
    grupos.append(np.append(valores_pixel[l*(k-1):,:],np.full(((l+n%k),1),k-1),axis=1))
    return grupos

def calcularCentroides(grupos,matriz):
    means=matriz   #Matriz de los centroides
    #Calcular medias (centroides)
    for i in range(k):
        for j in range(3):
            #Promedio de cada columna donde el grupo coincida con el cluster
            means[i][j]=np.mean(grupos[:,j][grupos[:,3]==i]) 
    return means        

def calcularDistancias(valores_pixel,centroides,n):
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
        centroides=calcularCentroides(grupos,np.zeros((k,3)))

        #Calcular distancias de pixeles a cada centroide
        nuevosGpos=calcularDistancias(valores_pixel,centroides,n)  

        #Reasignar grupos
        for j in range(n):
            grupos[j,3]=nuevosGpos[j]

    graficarClusters(grupos,centroides,k) 
    #Reasignar los valores al de los centroides
    for i in range(k):
        for j in range(3):
            grupos[:,:-1][grupos[:,3]==i]=centroides[i] 

    return grupos[:,:-1]

def graficarClusters(img_grupos,centroides,k):
    
    fig=plt.figure(figsize=(8,8))
    ax=fig.add_subplot(projection="3d")

    colores=['blue','red','green','cyan','yellow','orange','black','pink','brown','purple','olive','#AEBF00','#006E9A','#1818CE','#F6D21C','#A83314']
    #Graficar clusters
    for i in range(k):
        ax.scatter(img_grupos[:,0][img_grupos[:,3]==i],img_grupos[:,1][img_grupos[:,3]==i],img_grupos[:,2][img_grupos[:,3]==i],s=0.5,c=colores[i],label="Color %s" %str(i+1))
    ax.scatter(centroides[:,0],centroides[:,1],centroides[:,2],s=100,c="orange",marker="*")    
    plt.title("Clustering de colores")
    plt.xlabel("Canal R")
    plt.ylabel("Canal G")
    ax.set_zlabel("Canal B")
    plt.show()

def mostrarImagenSegmentada(imagen,img_segmentada,k):
    #Mostrar imagen resultante
    plt.figure(figsize=(10,4))
    plt.subplot(1,2,1),plt.imshow(cv2.cvtColor(imagen, cv2.COLOR_BGR2RGB))
    plt.title('Imagen Original'), plt.xticks([]), plt.yticks([])
    plt.subplot(1,2,2),plt.imshow(img_segmentada)
    plt.title('Imagen segmentada con %i clusters' %k), plt.xticks([]), plt.yticks([])
    plt.show()


#Cargo la imagen
imagen=cv2.imread('tornillos.png')

#Obtencion de los canales RGB
b,g,r=cv2.split(imagen)
b=b.reshape(-1)
g=g.reshape(-1)
r=r.reshape(-1)

k=0     #Numero de clusters

while(k<=1):
    k=int(input("Ingrese el numero de clusters: "))

#Convertir en arreglo de ternas
valores_pixel=np.array(list(zip(r,g,b)))
#Aplicar el clustering a los pixeles
img_segmentada=clustering(valores_pixel,k)

#Redimensionar imagen segmentada
img_segmentada=np.reshape(img_segmentada,(imagen.shape))
#Graficar segmentacion
mostrarImagenSegmentada(imagen,img_segmentada,k)
