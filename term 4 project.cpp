#include <algorithm>
#include <iostream>
#include <limits>
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

float dist(float x1, float y1, float x2, float y2) {
    /* Return distance */
    return std::sqrtf(std::powf(x1 - x2, 2) + std::powf(y1 - y2, 2));
}

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
};

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

    bool can_eat(Food* f) {
        /* Takes food. Returns true if a bacterium is close enough to eat this food */
        if (dist(m_x, m_y, f->m_x, f->m_y) <= f->m_radius) {
            return true;
        }
        return false;
    }

    void eat() {

    }

    void reproduce() {

    }
};

int main()
{
    srand(time(NULL)); // Random seed
    std::vector<Bacterium*> bacteria_container;
    std::vector<Bacterium*> bacteria_death_row;
    std::vector<Food*> food_container;
    std::vector<Food*> food_death_row;

    // Creat adams
    for (int i{}; i < ADAMS_N; ++i) {
        bacteria_container.push_back(new Bacterium);
    }

    int step{};
    float f_new_energy,
          closest_food_dist,
          curr_dist, // Current distance
          dx, dy, A, B, C, dist_to_move; // Temporary movement vars
    while (step < STEPS) {
        std::cout << step << "\n";
        // Movement
        if (food_container.size() > 0) {
            for (Bacterium* b : bacteria_container) {
                // Find closest food
                Food* closest_food{ food_container[0] };
                closest_food_dist = std::numeric_limits<float>::infinity();
                for (int i{ 1 }; i < food_container.size(); ++i) {
                    for (Food* f : food_container) {
                        curr_dist = dist(f->m_x, f->m_y, b->m_x, b->m_y);
                        if (curr_dist < closest_food_dist) {
                            closest_food = f;
                            closest_food_dist = curr_dist;
                        }
                    }
                }
                // Move
                A = closest_food->m_x - b->m_x;
                B = closest_food->m_y - b->m_y;
                C = std::sqrtf(std::powf(A, 2) + std::powf(B, 2));
                dist_to_move = std::min(b->m_speed, C - closest_food->m_radius); // No need to overshoot
                dx = A / C * dist_to_move;
                dy = B / A * dx;
                b->m_x += dx;
                b->m_y += dy;
            }
        }

        // Update food and bacteria energy
        for (Food* f : food_container) {
            std::vector<Bacterium*> consumers;
            for (Bacterium* b : bacteria_container) {
                if (b->can_eat(f)) {
                    consumers.push_back(b);
                }
            }
            f_new_energy = std::max(0.0f, f->m_energy - consumers.size());
            for (Bacterium* b : consumers) {
                b->m_energy += (f->m_energy - f_new_energy) / consumers.size();
            }
            f->m_energy = f_new_energy;
        }

        // Remove drained food
        for (Food* f : food_container) {
            if (f->m_energy <= 0) {
                food_death_row.push_back(f);
            }
        }
        for (Food* f1 : food_death_row) {
            for (int i{}; i < food_container.size(); ++i) {
                if (food_container[i] == f1) {
                    delete food_container[i];
                    food_container.erase(food_container.begin() + i);
                    break;
                }
            }
        }
        food_death_row.clear();

        // Bacteria death
        for (Bacterium* b : bacteria_container) {
            if (b->m_energy <= 0.0) {

            }
        }

        // Reproduction
        
        // Add food

        ++step;
    }
}