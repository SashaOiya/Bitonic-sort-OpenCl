import random

# Параметры
filename = 'random_numbers5.txt'  # Имя файла
num_numbers = 5000  # Количество случайных чисел
lower_bound = -10000    # Нижняя граница диапазона чисел
upper_bound = 10000  # Верхняя граница диапазона чисел

# Генерация случайных чисел
random_numbers = [random.randint(lower_bound, upper_bound) for _ in range(num_numbers)]

# Запись в файл
with open(filename, 'w') as file:
    file.write(f"{num_numbers}\n")
    for number in random_numbers:
        file.write(f"{number}\n")

print(f"Файл '{filename}' успешно создан.")
