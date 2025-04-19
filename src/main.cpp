// Include SFML for graphics and window rendering
#include <SFML/Graphics.hpp>

// Include standard libraries for vector and I/O
#include <vector>
#include <iostream>

// Include your custom classes for particles, constraints, and input handling
#include "particle.h"
#include "constraint.h"
#include "user.h"

// Set window dimensions
const int WIDTH = 1080;
const int HEIGHT = 640;

// Simulation constants
const float PARTICLE_RADIOUS = 10.0f;  // For drawing particles (unused in current code)
const float GRAVITY = 10.0f;           // Downward force applied to all particles
const float TIME_STEP = 0.1f;          // Time step for particle updates

// Grid setup: ROW x COL particles
const int ROW = 10;
const int COL = 10;
const float REST_DISTANCE = 30.0f;     // Distance between connected particles

int main() {
    // Output to confirm program has started
    std::cout << "Cloth simulation started" << std::endl;

    // Create an SFML window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cloth Simulation");
    window.setFramerateLimit(60); // Limit the frame rate for smoother simulation

    // Create a vector to store all the particles in the cloth
    std::vector<Particle> particles;

    // Create a vector to store constraints (connections) between particles
    std::vector<Constraint> constraints;

    // Initialize particles in a grid layout
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            // Calculate screen position for each particle
            float x = col * REST_DISTANCE + WIDTH / 3;
            float y = row * REST_DISTANCE + HEIGHT / 3;

            // Pin the top row (row == 0) so it doesn't fall
            bool pinned = (row == 0);

            // Add a new particle to the list
            particles.emplace_back(x, y, pinned);
        }
    }

    // Create constraints between neighboring particles (horizontal and vertical)
    for (int row = 0; row < ROW; row++) {
        for (int col = 0; col < COL; col++) {
            if (col < COL - 1) {
                // Horizontal constraint (left to right)
                constraints.emplace_back(&particles[row * COL + col], &particles[row * COL + col + 1]);
            }
            if (row < ROW - 1) {
                // Vertical constraint (top to bottom)
                constraints.emplace_back(&particles[row * COL + col], &particles[(row + 1) * COL + col]);
            }
        }
    }

    // Main simulation loop — runs until the window is closed
    while (window.isOpen()) {
        sf::Event event;

        // Process all events (like mouse clicks or window close)
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // Close window if user hits the X
            }

            // Handle mouse interaction with particles (e.g. tearing cloth)
            InputHandler::handle_mouse_click(event, particles, constraints);
        }

        // Apply physics to each particle
        for (auto& particle : particles) {
            particle.apply_force(sf::Vector2f(0, GRAVITY));          // Apply gravity
            particle.update(TIME_STEP);                              // Update position using Verlet integration
            particle.constrain_to_bounds(WIDTH, HEIGHT);             // Keep particles within screen bounds
        }

        // Enforce constraints multiple times for better accuracy (relaxes the cloth)
        for (size_t i = 0; i < 5; i++) {
            for (auto& constraint : constraints) {
                constraint.satisfy(); // Pull particles toward their correct distance
            }
        }

        // Clear the screen with black
        window.clear(sf::Color::Black);

        // OPTIONAL: Draw particles as filled circles (currently commented out)
        /*
        for (const auto& particle : particles) {
            sf::CircleShape circle(PARTICLE_RADIOUS);
            circle.setFillColor(sf::Color::White);
            circle.setPosition(particle.position.x - PARTICLE_RADIOUS, 
                                particle.position.y - PARTICLE_RADIOUS);
            window.draw(circle);
        }
        */

        // Draw particles as single white points
        for (const auto& particle : particles) {
            sf::Vertex point(particle.position, sf::Color::White);
            window.draw(&point, 1, sf::Points);
        }

        // Draw constraints (lines between connected particles)
        for (const auto& constraint : constraints) {
            if (!constraint.active) continue; // Skip inactive (broken) constraints

            // Create a line between two connected particles
            sf::Vertex line[] = {
                sf::Vertex(constraint.p1->position, sf::Color::White),
                sf::Vertex(constraint.p2->position, sf::Color::White),
            };
            window.draw(line, 2, sf::Lines);
        }

        // Display everything on the screen
        window.display();
    }
}
