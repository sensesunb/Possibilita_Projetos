# Arquivo principal

import numpy as np
import cv2
import pyautogui, sys
import Imagem
import Analise
import Mouse
import Janela

Webcam = cv2.VideoCapture(0)  #Cria objeto da WebCam

#Declaracao de variaveis importantes
Treshold_max = 100			  #Limite do Treshold, definido como 100 para simbolizar 100%
Treshold_quadrado = 15 	 	  #Treshold inicial do quadrado central, definido em 15%

def callback(x):
	Treshold_quadrado = x
	pass

Conta_Erro_Testa = 0          #conta a quantidade de quadros que o programa ficou sem achar círculo da testa
Conta_Erro_Queixo = 0		  #conta a quantidade de quadros que o programa ficou sem achar círculo do Queixo
Conta_Erro_OmbroD = 0		  #conta a quantidade de quadros que o programa ficou sem achar círculo do Ombro Direito
Conta_Erro_OmbroE = 0		  #conta a quantidade de quadros que o programa ficou sem achar círculo do Ombro Esquerdo
acelerador = 1				  #Determina a velocidade do cursor(parece com o que chamamos de sensibilidade)
passo = 0.1

#Definicao dos bits que representam cada tipo dos erros
BIT_TESTA = 0x01
BIT_QUEIXO = 0X02
BIT_OMBROD = 0X04
BIT_OMBROE = 0X08

#Declaracao de variaveis de controle
calibragem = 0				#variável usada para analisar a primeira imagem
#charCheckForEscKey = 0		#Vaariavel para detectar quando o Esc for teclado e encerrar o programa
controle = 0				#variavel utilizada para calibracao, enquanto for 0 o programa esta descalibrado
flag = 0

#testa se a Webcam foi aberta corretamente, caso contrario imprime mensagem de erro e encerra o programa
if Webcam.isOpened() == False:
	print("Erro: Webcam nao conectada\n")
	quit()


print ("Pressione barra de espaço quando os circulos estiverem no lugar certo\n")

#Criacao de Janelas e tackbars
cv2.namedWindow('HeadTracking') #cria a janela
cv2.createTrackbar('Tam. Quadrado','HeadTracking',Treshold_quadrado,Treshold_max,callback) #cria a Trackbar relacionada ao tamnaho do quadrado



#Loop principal do programa, se mantem aberto ate perder o contato com a webcam ou o usuario pressionar EscS
while (Webcam.isOpened()):
	
	Imagem_lida_com_sucesso, ImgOriginal = Webcam.read()    #le o frame seguinte e atualiza uma variavel booleana indicando se a leitura foi feita corretamente

	#caso a imagem nao for lida corretamente, imprime mensagem de erro e encerra o programa
	if Imagem_lida_com_sucesso == False:
		print ("Erro: Imagem nao foi lida da Webcam")
		break

	Treshold_quadrado = cv2.getTrackbarPos('Tam. Quadrado','HeadTracking')

	Circulos = Imagem.HoughCircles(ImgOriginal)
	Centros_ordenados = Analise.Ordena_centros(Circulos)
	MTQ = Analise.Acha_MTQ(Centros_ordenados)
	Janela.Desenha_Circulos(ImgOriginal,Centros_ordenados,MTQ)
	Janela.Desenha_Linhas(ImgOriginal, Centros_ordenados)
	Janela.Desenha_Quadrado(ImgOriginal, MTQ, Treshold_max, Treshold_quadrado)

	cv2.imshow('HeadTracking',ImgOriginal)

	#Testa se o usuario teclou Esc, se tiver teclado sai do programa
	if cv2.waitKey(1) == 27:
		break



Webcam.release()
cv2.destroyAllWindows()
