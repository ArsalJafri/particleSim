#ifndef USER_H
#define USER_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "particle.h"
#include "constraint.h"

// Maximum distance in pixels allowed for clicking on a constraint to tear it
const float CLICK_TOLERANCE = 5.0f;

// InputHandler class handles user input, like mouse clicks
class InputHandler {
public:
    // Called from main loop — checks if the mouse clicked near a constraint
    static void handle_mouse_click(const sf::Event& event, std::vector<Particle>& particles,
                                    std::vector<Constraint>& constraints) {
        // Only respond to left mouse button press
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

            // Get mouse click coordinates
            float mouse_x = static_cast<float>(event.mouseButton.x);
            float mouse_y = static_cast<float>(event.mouseButton.y);

            // Attempt to tear the cloth at the clicked position
            tear_cloth(mouse_x, mouse_y, particles, constraints);
        }
    }

private:
    // Calculates the shortest distance from a point (mouse) to a line segment (constraint)
    static float point_to_segment_distance(float px, float py,
                                           float x1, float y1, float x2, float y2) {
        // Vector from point A to point B
        float ABx = x2 - x1;
        float ABy = y2 - y1;

        // Vector from point A to the mouse point P
        float APx = px - x1;
        float APy = py - y1;

        // Vector from point B to P
        float BPx = px - x2;
        float BPy = py - y2;

        // Dot product of AB and AP
        float AB_AP = ABx * APx + ABy * APy;
        float AB_AB = ABx * ABx + ABy * ABy;
        float t = AB_AP / AB_AB;  // Projection scalar

        // Determine the closest point on the line segment to point P
        if (t < 0.0f) {
            // Closer to point A
            return std::sqrt(APx * APx + APy * APy);
        } else if (t > 1.0f) {
            // Closer to point B
            return std::sqrt(BPx * BPx + BPy * BPy);
        } else {
            // Projection point lies on the segment AB
            float proj_x = x1 + t * ABx;
            float proj_y = y1 + t * ABy;
            return std::sqrt((px - proj_x) * (px - proj_x) + (py - proj_y) * (py - proj_y));
        }
    }

    // Finds the nearest constraint (line) to the mouse click within tolerance
    static Constraint* find_nearest_constraint(float mouse_x, float mouse_y,
                                               const std::vector<Constraint>& constraints) {
        Constraint* nearest_constraint = nullptr;
        float min_distance = CLICK_TOLERANCE;

        for (const auto& constraint : constraints) {
            float distance = point_to_segment_distance(mouse_x, mouse_y,
                                constraint.p1->position.x, constraint.p1->position.y,
                                constraint.p2->position.x, constraint.p2->position.y);

            if (distance < min_distance) {
                min_distance = distance;
                nearest_constraint = const_cast<Constraint*>(&constraint); // safe here for UI use
            }
        }

        return nearest_constraint;
    }

    // Tries to find and deactivate a constraint near the mouse click
    static void tear_cloth(float mouse_x, float mouse_y, const std::vector<Particle>& particles,
                           std::vector<Constraint>& constraints) {
        Constraint* nearest = find_nearest_constraint(mouse_x, mouse_y, constraints);

        if (nearest) {
            nearest->deactivate();  // "Tear" the constraint
        }
    }
};

#endif // INPUT_HANDLER_H
