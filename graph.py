import matplotlib.pyplot as plt
import numpy as np

# Читаем матрицы для тестирования
matrixCircle = np.loadtxt('/Applications/Programs/Study/3kurs/6sem/CoUrCeWoRk/matrixCircle.csv')
matrix = np.loadtxt('/Applications/Programs/Study/3kurs/6sem/CoUrCeWoRk/matrix.csv')

# конвертируем матрицы в numpy arrays 
dataCircle = matrixCircle
data = matrix

# Создаем фигуру и оси для графиков
fig, axes = plt.subplots(nrows=1, ncols=2)

# Устанавливаем заголовки для графиков
axes[0].set_title('Circle Matrix')
axes[1].set_title('Matrix')

# Устанавливаем цветовые карты для графиков
cmap = plt.cm.gray
norm = plt.Normalize(vmin=dataCircle.min(), vmax=dataCircle.max())
rgba = cmap(norm(dataCircle))
axes[0].imshow(rgba, interpolation='nearest')

cmap = plt.cm.gray
norm = plt.Normalize(vmin=data.min(), vmax=data.max())
rgba = cmap(norm(data))
axes[1].imshow(rgba, interpolation='nearest')

# Показываем окно с графиками
plt.show()
