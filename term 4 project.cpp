#include <iostream>

// Hyperparameters
int STEPS; // Количество шагов симуляции.
float FOOD_DROP_PROB; // Вероятность добавления еды на уровень на каждом шаге
int	FOOD_MIN; // Минимальное значение энергии еды
int FOOD_MAX; // Максимальное значение энергии еды
int SUSTAINMENT; // Расход энергии на жизнеобеспечение на каждом шаге. Не подлежит мутации
int ADAMS_N; // Количество адамов
int MAX_ADAM_SPEED; // Максимальная скорость адамов
int MAX_ADAM_ENERGY; // Максимальная энергия адамов
float MUTATION_CHANCE; // Шанс мутации при размножении в долях. Рассчитывается для каждой характеристики отдельно
float MUTATION_STR; // Сила мутаций в долях от текущего значения


int main()
{
    std::cout << "Hello World!\n";
}