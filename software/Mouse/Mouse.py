import numpy as np
import cv2
import pyautogui, sys

img = cv2.imread ('teste.jpg')

while True:
	cv2.imshow('imagem',img)
	x, y = pyautogui.position()
	#positionStr = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
	#print(positionStr, '\n')
	#print('\b' * len(positionStr), end='', flush=True)
	
	a = cv2.waitKey(1)
	
	#A seta esquerda foi teclada
	if a == 81:
		x -= 1
		pyautogui.moveTo(x,y,2)
	#A seta de cima foi teclada
	if a == 82:
		y -= 1
		pyautogui.moveTo(x,y,2)
	#A seta da direita foi teclada
	if a == 83:
		x += 1
		pyautogui.moveTo(x,y,2)
	#A seta de baixo foi teclada
	if a == 84:
		y += 1
		pyautogui.moveTo(x,y,2)