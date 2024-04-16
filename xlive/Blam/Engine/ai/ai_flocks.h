#pragma once

#include "tag_files/tag_reference.h"
#include "tag_files/tag_block.h"
#include "tag_files/string_id.h"


#define k_max_flock_definitions_per_map 20
#define k_max_sinks_per_flock 10
#define k_max_sources_per_flock 10

enum e_flock_definition_flags : short
{
    flock_definition_flag_hard_boundaries_on_volume = FLAG(0),
    flock_definition_flag_flock_initially_stopped = FLAG(1)
};

// max count: k_max_sources_per_flock 10
struct flock_source
{
    real_vector3d position;
    real_euler_angles2d starting_yaw_pitch_degrees;
    float radius;
    float weight;    // probability of producing at this source
};
ASSERT_STRUCT_SIZE(flock_source, 28);

// max count: k_max_sinks_per_flock 10
struct flock_sink
{
    real_vector3d position;
    float radius;
};
ASSERT_STRUCT_SIZE(flock_sink, 16);

// max count: k_max_flock_definitions_per_map 20
struct flock_definition
{
    short bsp;                                  // Block Index: scenario_structure_bsp_reference
    short pad;
    short bounding_volume;                      // Block Index: scenario_trigger_volume

    e_flock_definition_flags flags;
    float ecology_margin;                       // (World Units) distance from ecology boundary that creature begins to be repulsed
    tag_block<flock_source> sources;
    tag_block<flock_sink> sinks;
    float production_frequency_boids_sec;       // How frequently boids are produced at one of the sources (limited by the max boid count)

    real_vector2d scale;
    tag_reference creature;     // crea
    short_bounds boid_count;

    // Explaination("Flock parameters", "Recommended initial values (for a sentinel-sized unit): 
    /*neighborhood radius = 6.0
        avoidance radius = 3
        forward scale = 0.5
        alignment scale = 0.5
        avoidance scale = 1.0
        leveling force scale = 0.1
        perception angle = 120
        average throttle = 0.75
        maximum throttle = 1.0
        position scale = 1.0
        position min radius = 3
        position max radius = 9")*/
    // Explaination("Let me give you a piece of free advice.", 
    /* "Flocks with a neighborhood radius of 0 don't FLOCK, per se (in the creature-creature interaction kind of way), 
    but they are much cheaper. The best thing is to have a non-zero radius for small flocks, and a zero radius for large flocks (or for 'flow-flocks', 
    ones with sources and sinks that are intended to create a steady flow of something). Note also that for flocks with a 0 neighborhood radius, the parameters for avoidance, 
    alignment, position and perception angle are ignored entirely.")*/

    float neighborhood_radius;                  // (World Units) distance within which one boid is affected by another
    float avoidance_radius;                     // (World Units) distance that a boid tries to maintain from another
    float forward_scale;                        // weight given to boid's desire to fly straight ahead
    float alignment_scale;                      // weight given to boid's desire to align itself with neighboring boids
    float avoidance_scale;                      // weight given to boid's desire to avoid collisions with other boids, when within the avoidance radius
    float leveling_force_scale;                 // weight given to boids desire to fly level
    float sink_scale;                           // weight given to boid's desire to fly towards its sinks
    float perception_angle_degrees;             // angle-from-forward within which one boid can perceive and react to another
    float average_throttle;                     // throttle at which boids will naturally fly
    float maximum_throttle;                     // maximum throttle applicable
    float position_scale;                       // weight given to boid's desire to be near flock center
    float position_min_radius;                  // (World Units) distance to flock center beyond which an attracting force is applied
    float position_max_radius;                  // (World Units) distance to flock center at which the maximum attracting force is applied 
    float movement_weight_threshold;            // The threshold of accumulated weight over which movement occurs
    float danger_radius;                        // (World Units) distance within which boids will avoid a dangerous object (e.g. the player)
    float danger_scale;                         // weight given to boid's desire to avoid danger

    // Explaination("Perlin noise parameters", "Recommended initial values: 
    // random offset scale = 0.2
    // offset period bounds = 1, 3")
    float random_offset_scale;                  // weight given to boid's random heading offset
    real_vector2d random_offset_period_seconds;
    string_id flock_name;
};
ASSERT_STRUCT_SIZE(flock_definition, 132);
