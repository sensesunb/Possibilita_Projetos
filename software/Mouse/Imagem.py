import numpy as np
import cv2
import Analise
import Janela


'''###############################################################################################
  Função que passa um filtro na imagem original com objetivo de diminuir a detecção de circulos
indesejáveis, e depois roda o algoritmo HoughCircles pra encontrar os círculos.

  -Parâmetros de entrada:
    .Endereço de uma classe do tipo Mat que armazena a imagem obtida pela webcam; 
    .Endereço de uma classe do tipo Mat que armazena a imagem na qual o HoughCircles irá
tralhar, ou seja, imagem obtida após a utilização de filtros pra preparar a imagem;
    .Endereço de um vetor de 3 floats (saída do HoughCircles), que vai armazenar o centro e o
raio dos círculos da última imagem analisada.

  -Parâmetros de saída:
    .Não há, pois a função altera as variáveis direto no endereço de memória delas.
'''###############################################################################################

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


'''###############################################################################################
  Função de calibragem do sistema.

  -Parâmetros de entrada:
    .Endereço de uma classe do tipo Mat que armazena a imagem obtida pela webcam; 
    .Endereço de um vetor de 3 floats (saída do HoughCircles), que vai armazenar o centro e o
raio dos círculos da última imagem analisada.
    .Endereço de um vetor de 3 floats (saída do HoughCircles), que vai armazenar o centro e o
raio dos círculos de maneira ordenada.
    .Endereço de uma classe do tipo point contendo as coordenadas do ponto medio inicial.

  -Parâmetros de saída:
    .Inteiro de controle, onde:
      controle = -1 -> encerra o programa
      controle = 0  -> programa nao calibrado
      controle = 1  -> programa calibrado
'''###############################################################################################

def calibragem(ImgOriginal, v3fCircles, v3fOrdenados_inicial, MTQ_inicial):

  controle = 0
  
  # Captura a imagem frame por frame
  ret, img = cap.read()

  v3fCircles = HoughCircles(ImgOriginal)
  v3fOrdenados_inicial = Analise.Ordena_centros(v3fCircles)
  MTQ_inicial = Analise.Acha_MTQ(v3fOrdenados_inicial)
  Janela.Desenha_Linhas(ImgOriginal,v3fOrdenados_inicial)
  Janela.Desenha_Circulos(ImgOriginal, v3fOrdenados_inicial, MTQ_inicial)
  Janela.Desenha_Quadrado(ImgOriginal, MTQ_inicial)
  Janela.Janelas(ImgOriginal)

  charKey = cv2.waitKey(10)

  if charKey == 32 & v3fOrdenados_inicial.size() == 4
    controle = 1

  if charKey == 27
    controle = -1

  return controle