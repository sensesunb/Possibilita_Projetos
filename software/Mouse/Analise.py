import numpy as np
import cv2
import math

def Ordena_centros(v3fCircles):

	if (v3fCircles == None):
		return

	a, b, c = v3fCircles.shape

	if (b < 4):
		return

	indT = 0;									#indice do circulo da testa
	indQ = 0;									#indice do circulo do queixo
	indOE = 0;									#indice do circulo do ombro esquerdo
	indOD = 0;									#indice do circulo do ombro direito
	centros_ordenados = v3fCircles.copy()

	#laço para encontrar o circulo da testa.
	for i in range(b):
		if (v3fCircles[0][i][1] <= v3fCircles[0][indT][1]):
			indT = i
			centros_ordenados[0][0][0] = v3fCircles[0][indT][0]
			centros_ordenados[0][0][1] = v3fCircles[0][indT][1]
			centros_ordenados[0][0][2] = v3fCircles[0][indT][2]

	v3fCircles[0][indT][0] = 641  #elimina as chances desse ponto ser pego em outro laço
	v3fCircles[0][indT][1] = 481  #elimina as chances desse ponto ser pego em outro laço

	#laço para encontrar o circulo da queixo. (considerando que o do queixo passa a ser o ponto mais alto depois do da testa)
	for i in range(b):
		if (v3fCircles[0][i][1] <= v3fCircles[0][indQ][1]):
			indQ = i
			centros_ordenados[0][1][0] = v3fCircles[0][indQ][0]
			centros_ordenados[0][1][1] = v3fCircles[0][indQ][1]
			centros_ordenados[0][1][2] = v3fCircles[0][indQ][2]

	v3fCircles[0][indQ][0] = 641  #elimina as chances desse ponto ser pego em outro laço
	v3fCircles[0][indQ][1] = 481  #elimina as chances desse ponto ser pego em outro laço

	#laço pra encontrar circulo do ombro esquerdo
	for i in range(b):
		if (v3fCircles[0][i][0] <= v3fCircles[0][indOE][0]):
			indOE = i
			centros_ordenados[0][2][0] = v3fCircles[0][indOE][0]
			centros_ordenados[0][2][1] = v3fCircles[0][indOE][1]
			centros_ordenados[0][2][2] = v3fCircles[0][indOE][2]

	v3fCircles[0][indOE][0] = 641  #elimina as chances desse ponto ser pego em outro laço
	v3fCircles[0][indOE][1] = 481  #elimina as chances desse ponto ser pego em outro laço

	#laço pra encontrar circulo do ombro direito
	for i in range(b):
		if (v3fCircles[0][i][0] <= v3fCircles[0][indOD][0]):
			indOD = i
			centros_ordenados[0][3][0] = v3fCircles[0][indOD][0]
			centros_ordenados[0][3][1] = v3fCircles[0][indOD][1]
			centros_ordenados[0][3][2] = v3fCircles[0][indOD][2]

	v3fCircles[0][indOD][0] = 641  #elimina as chances desse ponto ser pego em outro laço
	v3fCircles[0][indOD][1] = 481  #elimina as chances desse ponto ser pego em outro laço

	return centros_ordenados


def Acha_MTQ(centros_ordenados):
	if (centros_ordenados == None):
		return

	a, b, c = centros_ordenados.shape

	if (b < 4):
		return

	x = ((centros_ordenados[0][0][0] + centros_ordenados[0][1][0]) / 2)
	y = ((centros_ordenados[0][0][1] + centros_ordenados[0][1][1]) / 2)

	x = math.floor(x)
	y = math.floor(y)
	
	MTQ =  (x,y)

	return MTQ





