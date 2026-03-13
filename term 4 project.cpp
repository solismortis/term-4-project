#include <iostream>
#include <vector>

// Hyperparameters
//
int STEPS{ 10 }; // Total simulation steps
int LEVEL_SIZE{ 100 }; // Width and height of the level

float FOOD_DROP_PROB{ 0.1f }; // Probability of adding food to the level
int	FOOD_MIN{ 10 }; // Min food energy
int FOOD_MAX{ 100 }; // Max food energy
int SUSTAINMENT{ 1 }; // How much is spent on sustaining life per turn. Does not mutate

int ADAMS_N{ 5 }; // Number of adams
float ADAM_MAX_SPEED{ 5.0f }; // Adams' max speed
float ADAM_MAX_ENERGY{ 100.0f }; // Adams' max energy

float MUTATION_CHANCE{ 0.1f }; // Reproduction mutation probability 
float MUTATION_STR{ 0.1f }; // Reproduction mutation strength
//


int random_int(int min, int max) {
    int range;
    range = max - min + 1;
    return rand() % range + min;
}

float random_float(float min, float max) {
    // Bad for proper science
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

struct Bacterium {
    float m_x;
    float m_y;
    float m_speed;
    float m_max_energy;
    float m_energy; // Current energy

    Bacterium() {
        /* Adams constructor */
        m_x = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_y = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_speed = random_float(0.0f, ADAM_MAX_SPEED);
        m_max_energy = random_float(0.0f, ADAM_MAX_ENERGY);
        m_energy = m_max_energy;
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
            m_max_energy = parent->m_max_energy;
        }

        // Split parent energy
        m_energy = parent->m_energy / 2.0f;
    }

    void move() {
        // Find closest food
        
        // Move to it
    }

    bool can_eat(Food* f) {
        /* Takes food. Returns true if a bacterium is close enough to eat this food */
        if (std::sqrt(std::pow(m_x - f->m_x, 2)
                    + std::pow(m_y - f->m_y, 2)) <= f->m_radius) {
            return true;
        }
        return false;
    }

    void eat() {

    }

    void reproduce() {

    }

    ~Bacterium() {
        for (int i{ 0 }; i < bacteria_container.size(); ++i) {
            if (bacteria_container[i] == this) {
                bacteria_container.erase(bacteria_container.begin() + i);
            }
        }
    }

    void die() {
        delete this;
    }
};

struct Food {
    float m_x;
    float m_y;
    float m_energy;
    float m_radius;

    Food() {
        m_x = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_y = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_energy = random_float(FOOD_MIN, FOOD_MAX);
        m_radius = m_energy / 20.0f;
    }

    ~Food() {
        for (int i{ 0 }; i < food_container.size(); ++i) {
            if (food_container[i] == this) {
                food_container.erase(food_container.begin() + i);
            }
        }
    }
};

int main()
{
    srand(time(NULL)); // Random seed
    std::vector<Bacterium> bacteria_container;
    std::vector<Food> food_container;

    // Creat adams
    for (int i{}; i < ADAMS_N; ++i) {

    }

    int step{};
    float f_new_energy, f_prev_energy;
    while (step < STEPS) {
        std::cout << step << "\n";
        // Movement
        for (Bacterium b : bacteria_container) {
            b.move();
        }

        // Update food and bacteria energy
        for (Food f : food_container) {
            std::vector<Bacterium*> consumers;
            for (Bacterium b : bacteria_container) {
                if (b.can_eat(f)) {
                    consumers.push_back(b);
                }
            }
            // Each bacteria eats max 1 energy
            f_new_energy = std::max(0.0f, f->m_energy - consumers.size());
            f_prev_energy = f->m_energy;
            f->m_energy = f_new_energy;
            // Delete empty food
            if (f->m_energy <= 0) {
                delete f;
            }
            // Update bacteria energy (food component)
            for (Bacterium* b : consumers) {
                b->m_energy += (f_prev_energy - f_new_energy) / consumers.size();
            }
        }

        // Death
        
        // Reproduction
        
        // Add food
        ++step;
    }
}