/* Things to improve:
> When eating, take into account b->m_max_energy (right now some food gets wasted)
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

// Hyperparameters
//
int STEPS{ 100 }; // Total simulation steps
int LEVEL_SIZE{ 100 }; // Width and height of the level

float FOOD_DROP_PROB{ 0.3f }; // Probability of adding food per turn
int	FOOD_MIN{ 10 }; // Min food energy
int FOOD_MAX{ 200 }; // Max food energy
float SUSTAINMENT{ 0.1f }; // How much is spent on sustaining life per turn. Does not mutate

int ADAMS_N{ 5 }; // Number of adams
float ADAM_MIN_SPEED{ 3.0f };
float ADAM_MAX_SPEED{ 6.0f };
float ADAM_MAX_ENERGY{ 100.0f };

float MUTATION_CHANCE{ 0.1f }; // Reproduction mutation probability 
float MUTATION_STR{ 0.1f }; // Reproduction mutation strength
//

// Using the constructor to initialize with a seed
std::mt19937 mt(time(nullptr));

int random_int(int min, int max) {
    int range;
    range = max - min + 1;
    return mt() % range + min;
}

float random_float(float min, float max) {
    return min + static_cast<float>(mt()) / (static_cast<float>(mt.max() / (max - min)));
}

float dist(float x1, float y1, float x2, float y2) {
    /* Return distance */
    return std::sqrtf(std::powf(x1 - x2, 2.0f) + std::powf(y1 - y2, 2.0f));
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
        m_radius = m_energy / 5.0f;
    }
};

struct Bacterium {
    float m_x;
    float m_y;
    float m_max_speed;
    float m_max_energy;
    float m_energy; // Current energy

    Bacterium() {
        /* Adams constructor */
        m_x = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_y = random_float(0.0f, static_cast<float>(LEVEL_SIZE));
        m_max_speed = random_float(0.0f, ADAM_MAX_SPEED);
        m_max_energy = random_float(0.0f, ADAM_MAX_ENERGY);
        m_energy = random_float(0.0f, m_max_energy);
    }

    Bacterium(Bacterium* parent) {
        /* Child constructor */

        // Coords. Child's coords are close to parent's, but not exactly
        float max_spawn_spread{ 5.0f }; // How far a child will spawn relative to parent
        m_x = random_float(parent->m_x - max_spawn_spread, parent->m_x + max_spawn_spread);
        m_y = random_float(parent->m_y - max_spawn_spread, parent->m_y + max_spawn_spread);

        // Mutate speed
        if (random_float(0.0f, 1.0f) <= MUTATION_CHANCE) {
            m_max_speed = random_float(parent->m_max_speed * (1.0f - MUTATION_STR),
                                   parent->m_max_speed * (1.0f + MUTATION_STR));
        }
        else {
            m_max_speed = parent->m_max_speed;
        }

        // Mutate max energy
        if (random_float(0.0f, 1.0f) <= MUTATION_CHANCE) {
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
};

int main()
{
    std::ofstream frames_file("frames.json");
    json frames_json{ json::array() };
    json frame;
    json bacteria;

    std::vector<Bacterium*> bacteria_container;
    std::vector<Bacterium*> newborns;
    std::vector<Bacterium*> bacteria_death_row;
    std::vector<Food*> food_container;
    std::vector<Food*> food_death_row;
    int death_count{};
    int reproduction_events{};

    // Create adams
    for (int i{}; i < ADAMS_N; ++i) {
        bacteria_container.push_back(new Bacterium);
    }

    // Drop initial food
    food_container.push_back(new Food);

    int step{};
    float f_new_energy,
          closest_food_dist,
          curr_dist, // Current distance
          dx, dy, A, B, C, dist_to_move; // Temporary movement vars

    // Simulation
    while (step < STEPS) {
        std::cout << "Step: " << step << "\n";

        // Save frame
        frame["step"] = step;
        frame["death count"] = death_count;
        frame["reproduction events"] = reproduction_events;
        frame["bacteria"] = json::array();
        for (Bacterium* b : bacteria_container) {
            json bacteria_j{
                {"x", b->m_x},
                {"y", b->m_y},
                {"max_speed", b->m_max_speed},
                {"max_energy", b->m_max_energy},
                {"energy", b->m_energy} };
            frame["bacteria"].push_back(bacteria_j);
        }
        frame["food"] = json::array();
        for (Food* f : food_container) {
            json food_j{
                {"x", f->m_x},
                {"y", f->m_y},
                {"energy", f->m_energy},
                {"radius", f->m_radius}};
            frame["food"].push_back(food_j);
        }
        frames_json.push_back(frame);

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
                C = std::sqrtf(std::powf(A, 2.0f) + std::powf(B, 2.0f));
                dist_to_move = std::min(b->m_max_speed, C - closest_food->m_radius); // No need to overshoot
                dx = A / C * dist_to_move;
                dy = B / A * dx;
                b->m_x += dx;
                b->m_y += dy;
                // Drain bacteria energy
                b->m_energy -= dist_to_move * b->m_max_speed / 3.0f;
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
            if (f->m_energy <= 0.0f) {
                food_death_row.push_back(f);
            }
        }
        for (Food* f : food_death_row) {
            for (int i{}; i < food_container.size(); ++i) {
                if (food_container[i] == f) {
                    delete food_container[i];
                    food_container.erase(food_container.begin() + i);
                    break;
                }
            }
        }
        food_death_row.clear();

        // Sustainment
        for (Bacterium* b : bacteria_container) {
            b->m_energy -= SUSTAINMENT;
        }

        // Bacteria death
        for (Bacterium* b : bacteria_container) {
            if (b->m_energy <= 0.0f) {
                bacteria_death_row.push_back(b);
                ++death_count;
            }
        }
        for (Bacterium* b : bacteria_death_row) {
            for (int i{}; i < bacteria_container.size(); ++i) {
                if (bacteria_container[i] == b) {
                    delete bacteria_container[i];
                    bacteria_container.erase(bacteria_container.begin() + i);
                    break;
                }
            }
        }
        bacteria_death_row.clear();

        // Reproduction. Have to technically kill parent
        //
        for (Bacterium* b : bacteria_container) {
            if (b->m_energy >= b->m_max_energy) {
                newborns.push_back(new Bacterium(b));
                newborns.push_back(new Bacterium(b));
                bacteria_death_row.push_back(b);
                ++reproduction_events;
            }
        }
        // Kill parent
        for (Bacterium* b : bacteria_death_row) {
            for (int i{}; i < bacteria_container.size(); ++i) {
                if (bacteria_container[i] == b) {
                    delete bacteria_container[i];
                    bacteria_container.erase(bacteria_container.begin() + i);
                    break;
                }
            }
        }
        // Transfer newborns
        bacteria_death_row.clear();
        for (Bacterium* b : newborns) {
            bacteria_container.push_back(b);
        }
        newborns.clear();
        //
        
        // Add food
        if (random_float(0.0f, 1.0f) <= FOOD_DROP_PROB) {
            food_container.push_back(new Food);
        }

        // Increment step
        ++step;
    }

    frames_file << frames_json;
    frames_file.close();
    std::cout << "Simulation over\n";
}