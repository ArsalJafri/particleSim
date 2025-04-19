#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "particle.h"       // Uses Particle class to connect two points
#include <cmath>            // For std::hypot (distance calculation)
#include <limits>           // Not used here but good for numerical edge cases

// A Constraint connects two particles and tries to keep them at a fixed distance
class Constraint {
public:
    Particle *p1;           // Pointer to the first particle
    Particle *p2;           // Pointer to the second particle
    float initial_length;   // The original distance between the two particles
    bool active;            // Whether this constraint is currently in use (not torn)

    // Constructor: links two particles and records their initial separation
    Constraint(Particle *p1, Particle *p2) : p1(p1), p2(p2) {
        // Calculate Euclidean distance (hypotenuse) between the two particle positions
        initial_length = std::hypot(p2->position.x - p1->position.x,
                                    p2->position.y - p1->position.y);
        active = true;
    }

    // Enforces the constraint: moves particles to maintain their original distance
    void satisfy() {
        if (!active) return; // Skip if this constraint was deactivated (e.g. torn)

        // Vector from p1 to p2
        sf::Vector2f delta = p2->position - p1->position;

        // Current distance between particles
        float current_length = std::hypot(delta.x, delta.y);

        // Difference ratio used to compute how much to correct
        float difference = (current_length - initial_length) / current_length;

        // Split the correction between the two particles (half to each)
        sf::Vector2f correction = delta * 0.5f * difference;

        // Apply correction only to unpinned particles
        if (!p1->is_pinned) p1->position += correction;
        if (!p2->is_pinned) p2->position -= correction;
    }

    // Mark this constraint as broken — no longer enforces distance
    void deactivate() {
        active = false;
    }
};

#endif // CONSTRAINT_H
