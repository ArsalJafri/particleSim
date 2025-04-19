#ifndef PARTICLE_H
#define PARTICLE_H

// Include SFML vector and graphics functionality
#include <SFML/Graphics.hpp>

// A Particle represents a point in the cloth that can move (unless pinned)
class Particle {
public:
    // Current position of the particle
    sf::Vector2f position;

    // Previous position (used to calculate velocity in Verlet integration)
    sf::Vector2f previous_position;

    // Total force applied to the particle this frame (reset each update)
    sf::Vector2f acceleration;

    // Whether the particle is fixed in place or free to move
    bool is_pinned;

    // Constructor to initialize position, pinning state, and zero force
    Particle(float x, float y, bool pinned = false)
        : position(x, y), previous_position(x, y),
          acceleration(0, 0), is_pinned(pinned) {}

    // Adds a force to the particle (like gravity)
    void apply_force(const sf::Vector2f& force) {
        if (!is_pinned) {
            acceleration += force;
        }
    }

    // Updates particle position using **Verlet integration**
    void update(float time_step) {
        if (!is_pinned) {
            // Compute velocity as difference between current and previous position
            sf::Vector2f velocity = position - previous_position;

            // Save current position as previous (before moving it)
            previous_position = position;

            // Update position using velocity + force-based acceleration
            position += velocity + acceleration * time_step * time_step;

            // Reset acceleration (forces are applied again each frame)
            acceleration = sf::Vector2f(0, 0);
        }
    }

    // Makes sure the particle stays inside the simulation window
    void constrain_to_bounds(float width, float height) {
        if (position.x < 0) position.x = 0;
        if (position.x > width) position.x = width;
        if (position.y < 0) position.y = 0;
        if (position.y > height) position.y = height;
    }
};

#endif // PARTICLE_H
