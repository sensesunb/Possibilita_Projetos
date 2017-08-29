import numpy as np
import cv2

#pega um objeto de webcam, no caso pega o primeiro disponivel (indice 0)
cap = cv2.VideoCapture(1) 

while(True):
    # Captura a imagem frame por frame
    ret, img = cap.read()

    #trata a imagem
    cimg = cv2.medianBlur(img,5)                #filtro Blur
    cimg = cv2.cvtColor(cimg, cv2.COLOR_BGR2GRAY)   #Colorido para preto e branco

    #aplica o Hough Circles
    carac = cimg.shape  #pega as caracteristicas da imagem, retorna um vetor [linhas colunas canais] (nesse caso nao retorna os canais pq a imagem ta em preto e brano)
    rows = carac[0]     #armazena a qtd de linhas da img para passar de parametro na hough circles
    
    circles = cv2.HoughCircles(cimg,            #imagem de entrada
                   cv2.HOUGH_GRADIENT,  #algoritmo para detectar circulos
                   2,           #tamanho da imagem dividido por esse numero, no caso 2
                   rows/4,          #distância mínima em pixels entre os centros dos circulos detectados
                               param1=200,      #threshold máximo do detector Canny edge detector (chamado por cvHoughCircles)
                               param2=50,       #threshold mínimo do Canny edge detector (setado em metade do valor anterior)
                   minRadius=10,        #raio mínimo do circulo (qualquer circulo com raio menor não vai ser detectado)
                   maxRadius=40)        #raio máximo do círculo (qualquer círculo com raio maior não vai ser detectado)
    
    #desenha os circulos na imagem 
    if circles is not None: # Checa se foram encontrados circulos e se encontrados desenha-os na imagem 
        a, b, c = circles.shape
        for i in range(b):
            cv2.circle(img, (circles[0][i][0], circles[0][i][1]), circles[0][i][2], (0, 0, 255), 3, cv2.LINE_AA)
            cv2.circle(img, (circles[0][i][0], circles[0][i][1]), 2, (0, 255, 0), 3, cv2.LINE_AA)  # desenha o centro do circulo
    
    #Mostra o resultado na tela
    cv2.imshow('detected circles',img)

    #Espera o usuario teclar q para sair do while e encerrar o programa 
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

#Ao termino da execucao, libera a camera e mata as janelas 
cap.release()
cv2.destroyAllWindows()





