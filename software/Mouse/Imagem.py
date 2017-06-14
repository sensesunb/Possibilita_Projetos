import numpy as np
import cv2

def HoughCircles(ImgOriginal):
	
	#trata a imagem
    Img_Final = cv2.medianBlur(ImgOriginal,5)				#filtro Blur
    Img_Final = cv2.cvtColor(Img_Final, cv2.COLOR_BGR2GRAY)	#Colorido para preto e branco

    #aplica o Hough Circles
    carac = Img_Final.shape  #pega as caracteristicas da imagem, retorna um vetor [linhas colunas canais] (nesse caso nao retorna os canais pq a imagem ta em preto e brano)
    rows = carac[0]     #armazena a qtd de linhas da img para passar de parametro na hough circles
    
    circles = cv2.HoughCircles(Img_Final,			#imagem de entrada
			       cv2.HOUGH_GRADIENT,	#algoritmo para detectar circulos
			       2,			#tamanho da imagem dividido por esse numero, no caso 2
			       rows/4,			#distância mínima em pixels entre os centros dos circulos detectados
                               param1=200,		#threshold máximo do detector Canny edge detector (chamado por cvHoughCircles)
                               param2=50,		#threshold mínimo do Canny edge detector (setado em metade do valor anterior)
 			       minRadius=10,		#raio mínimo do circulo (qualquer circulo com raio menor não vai ser detectado)
			       maxRadius=40)		#raio máximo do círculo (qualquer círculo com raio maior não vai ser detectado)
    return circles

def calibragem():
	