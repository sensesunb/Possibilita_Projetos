import numpy as np
import cv2
import math


def Desenha_Circulos (Imagem, Circulos,MTQ):
	if Circulos is not None:
		a, b, c = Circulos.shape
		for i in range(b):
			cv2.circle(Imagem, (Circulos[0][i][0], Circulos[0][i][1]), Circulos[0][i][2], (0, 0, 255), 3, cv2.LINE_AA)
			cv2.circle(Imagem, (Circulos[0][i][0], Circulos[0][i][1]), 2, (0, 255, 0), 3, cv2.LINE_AA)  # desenha o centro do circulo

		cv2.circle(Imagem, (MTQ[0], MTQ[1]), 2, (0, 0, 0), 3, cv2.LINE_AA)

def Desenha_Linhas (Imagem, v3fCircles):
	if (v3fCircles == None):
		return

	a, b, c = v3fCircles.shape

	if (b < 4):
		return

	#Crias os pontos de interesse ja atualizando os pontos de acordo com os centros ordenados
	Testa  = (v3fCircles[0][0][0],v3fCircles[0][0][1])
	Queixo = (v3fCircles[0][1][0], v3fCircles[0][1][1])
	OmbroE = (v3fCircles[0][2][0], v3fCircles[0][2][1])
	OmbroD = (v3fCircles[0][3][0],v3fCircles[0][3][1])
	


	cv2.line(Imagem, Testa, Queixo, (0,0,255), 2, 8, 0)
	cv2.line(Imagem, OmbroE, OmbroD, (0,0,255), 2, 8, 0)


def Desenha_Quadrado(Imagem, MTQ, Treshold_max, Treshold_quadrado):
	if MTQ is not None:
		t = Treshold_quadrado / 100				#Treshold para o movimeno do mouse

		#Margens do quadrado de movimentos
		mx = math.floor((640 * t) / 2)			#parte inteira apenas
		my = math.floor((480 * t) / 2)			#parte inteira apenas

		#Criacao dos pontos dos vertices opostos do quadrado
		a = ((MTQ[0] + mx), (MTQ[1] + my))
		b = ((MTQ[0] - mx), (MTQ[1] - my))

		#Desenha o retangulo na imagem
		cv2.rectangle(Imagem, a, b, (255,0,0), 1, 8, 0)


