from __future__ import print_function
import keras
from keras.models import Sequential
from keras.models import model_from_json
from keras.layers import Dense, Dropout, Flatten, Activation
from keras.layers import Conv2D, MaxPooling2D
from keras import backend as K 
from keras.layers.normalization import BatchNormalization
import numpy as np
from matplotlib import pyplot as plt

np.random.seed(100)

batch_size = 128
num_classes = 18
epochs = 30

img_rows, img_cols = 65, 65

(_, _, _, _, _, _, a, b) = np.load('RecozhitionData.npy', encoding = 'bytes')

(x_train, x_test, _), (y_train, y_test, _) = np.split(a, [1404, 1638]), np.split(b, [1404, 1638])

x_train = x_train.astype('float32')
x_test = x_test.astype('float32')
x_train /= 255
x_test /= 255

json_file = open('uyir_model.json', 'r')
uyir_model = json_file.read()
json_file.close()
uyir_model = model_from_json(uyir_model)

uyir_model.load_weights("uyir_weights.h5")
print("Loaded uyir_model from disk")

uyir_model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=keras.optimizers.Adam(),
              metrics=['accuracy'])

json_file = open('mei_model.json', 'r')
mei_model = json_file.read()
json_file.close()
mei_model = model_from_json(mei_model)

mei_model.load_weights("mei_weights.h5")
print("Loaded mei_model from disk")

mei_model.compile(loss=keras.losses.categorical_crossentropy,
              optimizer=keras.optimizers.Adam(),
              metrics=['accuracy'])

labelMap = np.zeros((18, 13), dtype = int)

I = 0
for i in range(0, 18):
    for j in range(0, 13):
        labelMap[i][j] = I
        I += 1

print(labelMap)
np.savetxt('uyir_mei_label.csv', labelMap, delimiter=' ')

         
def predict(imgArr):
    tempArr = imgArr.reshape(1, 65, 65, 1)
    y = uyir_model.predict_classes(tempArr)[0]
    x = mei_model.predict_classes(tempArr)[0]
    return(labelMap[x][y])

x = predict(x_train[1])
print(x)
plt.imshow(x_train[1])
plt.show()