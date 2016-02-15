/**
 * Restarting simulations
 * 
 * This example demonstrates how to restart a simulation
 * using a binary file. A shearing sheet ring simulation is used, but
 * the same method can be applied to any other type of simulation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"

void heartbeat(struct reb_simulation* const r);
double coefficient_of_restitution_bridges(const struct reb_simulation* const r, double v);

int main(int argc, char* argv[]) {
	struct reb_simulation* r = reb_create_simulation_from_binary("moon.bin");
	// Setup constants
	r->opening_angle2	= .5;					// This determines the precission of the tree code gravity calculation.
	r->integrator			= REB_INTEGRATOR_SEI;
	r->boundary			= REB_BOUNDARY_SHEAR;
	r->gravity			= REB_GRAVITY_COMPENSATED;
	r->collision			= REB_COLLISION_DIRECT;
	double OMEGA 			= 0.00011755923;	// 1/s
	r->ri_sei.OMEGA 		= OMEGA;
	r->G 				= 6.67428e-11;		// N / (1e-5 kg)^2 m^2
	r->softening 			= 13.;			// m
	r->dt 				= 1e-6*2.*M_PI/OMEGA;	// s
	r->heartbeat			= heartbeat;	// function pointer for heartbeat
	double boxsize 			= 5000000;		// m
	reb_configure_box(r, boxsize, 1, 1, 1);
	r->nghostx = 0;
	r->nghosty = 0;
	r->nghostz = 0;
	
	// Use Bridges et al coefficient of restitution.
	r->coefficient_of_restitution = coefficient_of_restitution_bridges;
	// When two particles collide and the relative velocity is zero, the might sink into each other in the next time step.
	// By adding a small repulsive velocity to each collision, we prevent this from happening.
	r->minimum_collision_velocity = r->particles[0].r*OMEGA*0.001;  // small fraction of the shear accross a particle

	// Give all ring paricles velocity
	for (int i=0;i<r->N;i++) r->particles[i].vx = 0., r->particles[i].vy = 0., r->particles[i].vz = 0.;

	// Add second moonlet
	int i = 0;
	int number = 500;
	double dela  = -10000;             // Impactor Delta a
	double dele  = -10000;             // Impactor a Delta e
	double phase  = -M_PI/2.;   	   // Impactor start phase so that it goes through (0,0)
	while(i<number){
		struct reb_particle im = {0};
		im.x 		= r->particles[i].x + dela - (dele * cos(phase))-700.;
		im.y 		= r->particles[i].y + 2.*dele*sin(phase) - (3./2.)*dela*phase;
		im.z 		= r->particles[i].z+1.0;
		im.vx 		= r->particles[i].vx+OMEGA*dele*sin(phase);
		im.vy 		= r->particles[i].vy+2.*OMEGA*dele*cos(phase)-(3./2.)*dela*OMEGA;
		im.vz 		= r->particles[i].vz;
		im.r 		= 130.;						
		im.m 		= r->particles[i].m; 	
		im.id		= i+number;
		reb_add(r, im);
		i ++;
	}

	reb_integrate(r, 5.*2.*M_PI/r->ri_sei.OMEGA);
}

// This example is using a custom velocity dependend coefficient of restitution
double coefficient_of_restitution_bridges(const struct reb_simulation* const r, double v){
	// assumes v in units of [m/s]
	double eps = 0.32*pow(fabs(v)*100.,-0.234);
	if (eps>1) eps=1;
	if (eps<0) eps=0;
	return eps;
}

void heartbeat(struct reb_simulation* const r){
	if (reb_output_check(r, 1e-6*2.*M_PI/r->ri_sei.OMEGA)){
		reb_output_timing(r, 0);

	}
		if (reb_output_check(r, 1e-2*2.*M_PI/r->ri_sei.OMEGA)){
		reb_output_ascii(r, "positions.txt");
	}
}
