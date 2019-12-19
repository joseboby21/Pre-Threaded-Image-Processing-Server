import cv2
import sys
name='image'+str(sys.argv[1])+'.jpg'
face_cascade = cv2.CascadeClassifier('/home/ubuntu/image_server/server/haarcascade_frontalface_default.xml')
img = cv2.imread(name)
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
faces = face_cascade.detectMultiScale(gray, 1.1, 4)
for (x, y, w, h) in faces:
    cv2.rectangle(img, (x, y), (x+w, y+h), (255, 0, 0), 2)
newName = 'image2_'+str(sys.argv[1])+'.jpg'
cv2.imwrite(newName,img)




