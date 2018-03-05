/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <stdexcept>

#include "particle_filter.h"

using namespace std;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
    // Sets the number of particles. Initialize all particles to first position (based on estimates of
    //   x, y, theta and their uncertainties from GPS) and all weights to 1.
    // Adds random Gaussian noise to each particle.
    num_particles = 20;
    particles = std::vector<Particle>();
    // These lines create\ a normal (Gaussian) distribution for x, y and theta
    default_random_engine gen;
    normal_distribution<double> dist_x(x, std[0]);
    normal_distribution<double> dist_y(y, std[1]);
    normal_distribution<double> dist_theta(theta, std[2]);
    
    Particle p;
    for(int i=0; i<num_particles; i++)
    {
        p.id = i;
        p.x = dist_x(gen);
        p.y = dist_y(gen);
        p.theta = dist_theta(gen);
        p.weight = 1.0;
        particles.push_back(p);
    };
    
    is_initialized = true;
    
}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
    // Predicts the particles next location, given the velocity and yaw rate.
    default_random_engine gen;
    normal_distribution<double> dist_x(0.0, std_pos[0]);
    normal_distribution<double> dist_y(0.0, std_pos[1]);
    normal_distribution<double> dist_theta(0.0, std_pos[2]);
    
    for(Particle& p : particles)
    {
        // Case when the yaw rate is zero (or close)
        if(abs(yaw_rate) < 1e-6)
        {
            p.x = p.x + velocity * cos(p.theta) * delta_t + dist_x(gen);
            p.y = p.y + velocity * sin(p.theta) * delta_t + dist_y(gen);
            p.theta = p.theta + dist_theta(gen);
        }
        // General case
        else
        {
            p.x = p.x + ( velocity / yaw_rate ) *
            (sin(p.theta + delta_t * yaw_rate) - sin(p.theta)) + dist_x(gen);
            
            p.y = p.y + ( velocity / yaw_rate ) *
            (cos(p.theta) - cos(p.theta + delta_t * yaw_rate)) + dist_y(gen);
            
            p.theta = p.theta + yaw_rate * delta_t + dist_theta(gen);
        }
    }
}

std::vector<int> ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs> observations) {
    // Finds the predicted measurement that is closest to each observed measurement and assigns the
    //   observed measurement to this particular landmark.
    int closest; // This corresponds to a landmark id
    std::vector<int> output(observations.size());
    
    LandmarkObs p, o;
    
    for(int i = 0; i < observations.size(); i++)
    {
        o = observations[i];
        closest = -1;
        
        for(int j = 0; j < predicted.size(); j++)
        {
            p = predicted[j];
            if(closest == -1) closest = j;
            bool is_closer = dist(p.x, p.y, o.x, o.y) < dist(o.x, o.y, predicted[closest].x, predicted[closest].y);
            
            closest = is_closer ? j : closest;
        }
        
        output[i] = predicted[closest].id;
    }
    
    return(output);
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[],
                                   const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
    // Updates the weights of each particle using a mult-variate Gaussian distribution.
    // NOTE: The observations are given in the VEHICLE'S coordinate system. The particles are located
    //   according to the MAP'S coordinate system. We need transform between the two systems.
    //   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
    double sum_weights = 0.0;
    
    auto predicted_global = std::vector<LandmarkObs>();
    auto obs_global = std::vector<LandmarkObs>();
    
    std::vector<double> sense_x(observations.size());
    std::vector<double> sense_y(observations.size());
    
    for(Particle& p : particles)
    {
        // Converting map landmarks to particle coodinates
        
        
        // Creating the global observation vector
        obs_global.clear();
        for(auto o : observations)
        {
            LandmarkObs temp_o = { o.id,
                p.x + o.x * cos(p.theta) - o.y * sin(p.theta),
                p.y + o.x * sin(p.theta) + o.y * cos(p.theta),
            };
            obs_global.push_back(temp_o);
            
        }
        
        // Creating the global prediction vector
        predicted_global.clear();
        for(auto l : map_landmarks.landmark_list)
        {
            LandmarkObs temp_l = {l.id_i, l.x_f, l.y_f};
            predicted_global.push_back(temp_l);
        }
        
        // Here we associate the observations to the nearest landmark
        auto associations = dataAssociation(predicted_global, obs_global);
        for(int k = 0; k < obs_global.size(); k++)
        {
            sense_x[k] = obs_global[k].x;
            sense_y[k] = obs_global[k].y;
        }
        p = SetAssociations(p, associations, sense_x, sense_y);
        
        // We are finally ready to set the new particle weights
        p.weight = 1.0;
        
        for(int j = 0; j < obs_global.size(); j++)
        {
            double gauss_norm = 2 * M_PI * std_landmark[0] * std_landmark[1];
            double x_obs = obs_global[j].x;
            double y_obs = obs_global[j].y;
            double mu_x = predicted_global[associations[j] - 1].x; // Landmark id to location in the landmark vector:
            double mu_y = predicted_global[associations[j] - 1].y; // ... location = id - 1
            double exponent = pow(x_obs - mu_x, 2) / (2 * pow(std_landmark[0], 2)) +
            pow(y_obs - mu_y, 2) / (2 * pow(std_landmark[1], 2));
            
            
            p.weight *= exp(-exponent) / gauss_norm;
        }
        
        sum_weights += p.weight;
    }
    sum_weights = sum_weights < 1e-3 ? 1e-3 : sum_weights;
    
    // Weights to probabilities
    for(Particle& p : particles)
        p.weight = p.weight / sum_weights;
    
}


void ParticleFilter::resample() {
    // Resamples particles with replacement with probability proportional to their weight.
    std::vector<Particle> prev_particles = particles;
    particles = std::vector<Particle>();
    
    // Defining the weights vector
    vector<double> weights = vector<double>();
    for(Particle p : prev_particles)
        weights.push_back(p.weight);
    
    
    default_random_engine gen;
    discrete_distribution<int> sampler(weights.begin(), weights.end());
    
    for(int i = 0; i < num_particles; i++)
        particles.push_back(prev_particles[sampler(gen)]);
}

Particle ParticleFilter::SetAssociations(Particle particle, std::vector<int> associations, std::vector<double> sense_x, std::vector<double> sense_y)
{
    //particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
    // associations: The landmark id that goes along with each listed association
    // sense_x: the associations x mapping already converted to world coordinates
    // sense_y: the associations y mapping already converted to world coordinates
    
    //Clear the previous associations
    particle.associations.clear();
    particle.sense_x.clear();
    particle.sense_y.clear();
    
    particle.associations= associations;
    particle.sense_x = sense_x;
    particle.sense_y = sense_y;
    
    return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
    vector<int> v = best.associations;
    stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
    vector<double> v = best.sense_x;
    stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
    vector<double> v = best.sense_y;
    stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
