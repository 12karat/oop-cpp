import matplotlib.pyplot as plt
import numpy as np

n_values = [1000, 5000, 10000, 50000, 100000, 500000, 1000000]
times = [0.001, 0.017, 0.05, 1.231, 8.214, 197.093, 788.372] 

# Функция сложности O(n^2)
def f(n):
    return n**2

c = times[-1] / f(n_values[-1])

n_predict = 2000000
t_predict = c * f(n_predict)

n_theory = np.geomspace(1000, n_predict, 100) 
t_theory = c * f(n_theory)

plt.figure(figsize=(12, 8))

plt.plot(n_theory, t_theory, 'r-', alpha=0.6, linewidth=2, label=r'$f(n) = O(n^2)$')

plt.plot(n_values, times, 'b-o', markersize=8, label='Реальные замеры')

plt.plot(n_predict, t_predict, 'rs', markersize=9, label=f'Прогноз (N=2M): {t_predict:.1f}s')

for n, t in zip(n_values, times):
    plt.text(n, t * 1.5, f"{t}s", color='blue', ha='center', fontsize=9, fontweight='bold')
plt.text(n_predict, t_predict * 1.5, f"{t_predict:.1f}s", color='red', ha='center', fontsize=10, fontweight='bold')

plt.xscale('log')
plt.yscale('log')
plt.grid(True, which="both", ls='--', alpha=0.5)

plt.title('Анализ сложности алгоритма Иосифа (Array)', fontsize=14)
plt.xlabel('Количество человек (N)', fontsize=12)
plt.ylabel('Время выполнения (сек)', fontsize=12)

all_n = n_values + [n_predict]
plt.xticks(all_n, [str(v) for v in all_n], rotation=45)

plt.legend()
plt.tight_layout()
plt.show()