import cv2
import math
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

height1, width1, channels1 = img.shape
blank_imageb = np.zeros((height1,width1,3), np.uint8)
blank_imageg = np.zeros((height1,width1,3), np.uint8)
blank_imagey = np.zeros((height1,width1,3), np.uint8)

blank_imageb[:, 0:width1] = (255, 0, 0)      # (B, G, R)
blank_imageg[:, 0:width1] = (0, 255, 0)      # (B, G, R)
blank_imagey[:, 0:width1] = (0, 255, 255)      # (B, G, R)

blue = cv2.bitwise_and(blank_imageb,blank_imageb,mask=imagemaskb)
green = cv2.bitwise_and(blank_imageg,blank_imageg,mask=imagemaskg)
yellow = cv2.bitwise_and(blank_imagey,blank_imagey,mask=imagemaskyellow)

Test = cv2.bitwise_or(blue,green)
Test1 =cv2.bitwise_or(Test,yellow)

cv2.imshow("Test", Test1)

#TRANSFORMATIONS MANUELLES D’UNE IMAGE

height = img.shape[0]
width = img.shape[1]
channels = img.shape[2]
imgTransform = img
'''
for x in range(0, (int)(width/2)): # Applique une transformation sur WxH /2
    for y in range (0, (int)(height/2)):
            imgTransform[y,x][0] *= 0.5 # tend vers 0 assombrir l'image
            imgTransform[y,x][1] *= 0.5
            imgTransform[y,x][2] *= 0.5
cv2.imshow("Transformation␣manuelle␣de␣l’image", imgTransform)
cv2.waitKey(0)


for x in range(0, (int)(width)): 
    for y in range (0, height):
            imgTransform[y,x][0] *= 0 #R
            imgTransform[y,x][1] *= 1 #G # tend vers 0 assombrir l'image
            imgTransform[y,x][2] *= 0 #B
cv2.imshow("Transformation␣manuelle␣de␣l’image", imgTransform)
cv2.waitKey(0)

'''
step=0

for x in range(0, (int)(width)):
    step+=1/width
    for y in range (0, height):    
            imgTransform[y,x][0] *= 1-step #R
            imgTransform[y,x][1] *= 1-step #G # tend vers 0 assombrir l'image
            imgTransform[y,x][2] *= 1-step #B
cv2.imshow("Transformation␣manuelle␣de␣l’image", imgTransform)
cv2.waitKey(0)
Dis = 2
theta=0

for theta in range(0,math.pi):
    x = Dis*math.cos(theta)
    y = Dis*math.sin(theta)
    
            for x in range (int(height/2), height):
         
            imgTransform[y,x][0] *= 1
            imgTransform[y,x][1] *= 0
            imgTransform[y,x][2] *= 0
           
    for y in range (int(height/2), height):
         
            imgTransform[y,x][0] *= 1
            imgTransform[y,x][1] *= 0
            imgTransform[y,x][2] *= 0


       
  
cv2.imshow("Transformation␣manuelle␣de␣l’image", imgTransform)
cv2.waitKey(0)








