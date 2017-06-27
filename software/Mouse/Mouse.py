import numpy as np
import cv2
import pyautogui, sys

def clique_direito():

	if Conta_Erro_OmbroD > 30 & Conta_Erro_OmbroE < 10:
		pyautogui.click (button = 'right')

def clique_esquerdo():
	if Conta_Erro_OmbroE > 30 & Conta_Erro_OmbroD < 10:
		pyautogui.mouseDown(button = 'left')

	if Conta_Erro_OmbroE == 0:
		pyautogui.mouseUp(button = 'left')

def clique_duplo():
	if Conta_Erro_OmbroE > 30 & Conta_Erro_OmbroD > 30:
		pyautogui.doubleClick(button = 'left')

def move(direction):
	x, y = pyautogui.position()

	#Movimenta o mouse para baixo
	if direction == 0:
		y -= acelerador
		pyautogui.moveTo(x,y)

	#Movimenta o mouse para cima
	if direction == 1:
		y += acelerador
		pyautogui.moveTo(x,y)

	#Movimenta o mouse para a direita
	if direction == 2:
		x += acelerador
		pyautogui.moveTo(x,y)

	#Movimenta o mouse para a esquerda
	if direction == 3:
		x -= acelerador
		pyautogui.moveTo(x,y)