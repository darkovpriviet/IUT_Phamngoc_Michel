import cv2
import numpy as np
from urllib.request import urlopen
req = urlopen("http://www.vgies.com/downloads/robocup.png")
arr = np.asarray(bytearray(req.read()), dtype=np.uint8)
img = cv2.imdecode(arr, -1)
cv2.imshow("RoboCup␣image", img)
cv2.waitKey(0)

B, G, R = cv2.split(img)
'''
cv2.imshow("original", img)
cv2.waitKey(0)
cv2.imshow("blue", B)  
cv2.waitKey(0)
cv2.imshow("Green", G)
cv2.waitKey(0)
cv2.imshow("Red", R)
cv2.waitKey(0)
'''


#converting the image to HSV color space using cvtColor function
imagehsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
H, S, V = cv2.split(imagehsv)
'''
cv2.imshow("Hue", H)
cv2.waitKey(0)
cv2.imshow("Saturation", S)
cv2.waitKey(0)
cv2.imshow("Value", V)
cv2.waitKey(0)
'''
#Hue est flouté

#Definition des limites basses et hautes de la couleur jaune en HSV
#A noter que le jaune se situe vers les 25 degres dans la roue de couleur HSV en H
lower_yellow = np.array([20, 100, 100])
upper_yellow = np.array([30,255,255])
#Masquage de l’image HSV pour ne garder que les zones jaunes
imagemaskyellow = cv2.inRange(imagehsv, lower_yellow, upper_yellow)
cv2.imshow("Image␣Masque␣Jaune", imagemaskyellow)
cv2.waitKey(0)


lower_g = np.array([30, 20, 40])
upper_g = np.array([100,200,200])
#Masquage de l’image HSV pour ne garder que les zones jaunes
imagemaskg = cv2.inRange(imagehsv, lower_g, upper_g)
cv2.imshow("Image␣Masque␣g", imagemaskg)
cv2.waitKey(0)


lower_b = np.array([10, 0, 0])
upper_b = np.array([255,90,90])
#Masquage de l’image HSV pour ne garder que les zones jaunes
imagemaskb = cv2.inRange(imagehsv, lower_b, upper_b)
cv2.imshow("Image␣Masque␣b", imagemaskb)
cv2.waitKey(0)

height, width, channels = img.shape
blank_imageb = np.zeros((height,width,3), np.uint8)
blank_imageg = np.zeros((height,width,3), np.uint8)
blank_imagey = np.zeros((height,width,3), np.uint8)

blank_imageb[:, 0:width] = (255, 0, 0)      # (B, G, R)
blank_imageg[:, 0:width] = (0, 255, 0)      # (B, G, R)
blank_imagey[:, 0:width] = (0, 255, 255)      # (B, G, R)

blue = cv2.bitwise_and(blank_imageb,blank_imageb,mask=imagemaskb)
green = cv2.bitwise_and(blank_imageg,blank_imageg,mask=imagemaskg)
yellow = cv2.bitwise_and(blank_imagey,blank_imagey,mask=imagemaskyellow)

Test = cv2.bitwise_or(blue,green)
Test1 =cv2.bitwise_or(Test,yellow)

cv2.imshow("Test", Test1)



