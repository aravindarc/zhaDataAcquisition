import numpy as np 
from PIL import Image
from matplotlib import pyplot as plt

dataArray = np.zeros((1976, 65, 65), dtype = int)
labelArray = np.zeros(1976, dtype = int)

c = 0
for i in range(0, 7):
    for j in range(0, 247):
        img = Image.open('ResultantStorage/' + str(i) + '/' + str(j) + '.png')
        array = np.array(img)
        dataArray[c] = array
        labelArray[c] = j
        c = c + 1

plt.imshow(dataArray[1481])
print labelArray[1481]
plt.show()