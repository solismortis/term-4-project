#include <iostream>

// Hyperparameters
int STEPS{ 100 }; // Total simulation steps
float FOOD_DROP_PROB{ 0.1f }; // Probability of adding food to the level
int	FOOD_MIN{ 10 }; // Min food energy
int FOOD_MAX{ 100 }; // Max food energy
int SUSTAINMENT{ 1 }; // How much is spent on sustaining life per turn. Does not mutate
int ADAMS_N{ 5 }; // Number of adams
float ADAM_MAX_SPEED{ 5.0f }; // Adams' max speed
float ADAM_MAX_ENERGY{ 100.0f }; // Adams' max energy
float MUTATION_CHANCE{ 0.1f }; // Reproduction mutation probability 
float MUTATION_STR{ 0.1f }; // Reproduction mutation strength

int LEVEL_SIZE{ 100 }; // Height and width of the level

int random_int(int min, int max) {
    int range;
    range = max - min + 1;
    return rand() % range + min;
}

float random_float(float min, float max) {
    // Bad for proper science
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

class Bacterium {
    float m_x;
    float m_y;
    float m_speed;
    float m_max_energy;
    float m_energy; // Current energy
    Bacterium* m_parent;
public:
    Bacterium() {
        /* Adams constructor */
        m_x = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_y = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_speed = random_float(0.0f, ADAM_MAX_SPEED);
        m_max_energy = random_float(0.0f, ADAM_MAX_ENERGY);
        m_energy = m_max_energy;
        m_parent = nullptr;
    }

    Bacterium(Bacterium* parent) {
        /* Child constructor */
        // Coords. Child's coords are close to parent's, but not exactly
        float max_spawn_spread{ 1.0f }; // How far a child will spawn relative to parent
        m_x = random_float(parent->m_x - max_spawn_spread, parent->m_x + max_spawn_spread);
        m_y = random_float(parent->m_y - max_spawn_spread, parent->m_y + max_spawn_spread);
        // Mutate speed
        if (random_float(0.0f, 1.0f) >= MUTATION_CHANCE) {
            m_speed = random_float(parent->m_speed * (1.0f - MUTATION_STR),
                                   parent->m_speed * (1.0f + MUTATION_STR));
        }
        else {
            m_speed = parent->m_speed;
        }
        // Mutate max energy
        if (random_float(0.0f, 1.0f) >= MUTATION_CHANCE) {
            m_max_energy = random_float(parent->m_max_energy * (1.0f - MUTATION_STR),
                                        parent->m_max_energy * (1.0f + MUTATION_STR));
        }
        else {

        }
        // Split parent energy
    }
};

class Food {

};

int main()
{
    srand(time(NULL)); // Random seed

    int step{ 1 };
    while (true) {
        // Новые координаты бактерий
        
        // Энергия капель
        
        // Энергия бактерий
        
        // Смерть
        
        // Размножение
        
        // Добавление еды
    }
}