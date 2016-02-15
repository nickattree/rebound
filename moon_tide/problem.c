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
double coefficient_of_restitution(const struct reb_simulation*r, double v){
	return 0.01;
}

int main(int argc, char* argv[]) {
	struct reb_simulation* r = reb_create_simulation_from_binary("position.bin");
	// Setup constants
	r->opening_angle2	= .5;					// This determines the precission of the tree code gravity calculation.
	r->integrator			= REB_INTEGRATOR_SEI;
	r->boundary			= REB_BOUNDARY_SHEAR;
	r->gravity			= REB_GRAVITY_COMPENSATED;
	r->collision			= REB_COLLISION_DIRECT;
	r->coefficient_of_restitution 	= coefficient_of_restitution;
	double OMEGA 			= 0.00011755923;	// 1/s
	r->ri_sei.OMEGA 		= OMEGA;
	r->G 				= 6.67428e-11;		// N / (1e-5 kg)^2 m^2
	r->softening 			= 13.;			// m
	r->dt 				= 1e-6*2.*M_PI/OMEGA;	// s
	r->heartbeat			= heartbeat;	// function pointer for heartbeat
	double boxsize 			= 12000;		// m
	reb_configure_box(r, boxsize, 1, 1, 1);
	r->nghostx = 0;
	r->nghosty = 0;
	r->nghostz = 0;
	
	// Give all ring paricles velocity
	//for (int i=0;i<r->N;i++) r->particles[i].x += 400.;

	r->minimum_collision_velocity = r->particles[0].r*OMEGA*0.001;  // small fraction of the shear accross a particle

	reb_integrate(r, 2.*2.*M_PI/r->ri_sei.OMEGA);
	reb_output_binary(r, "moon.bin");
	reb_free_simulation(r);
	r = NULL;
}

void heartbeat(struct reb_simulation* const r){
	if (reb_output_check(r, 1e-6*2.*M_PI/r->ri_sei.OMEGA)){
		reb_output_timing(r, 0);
		//reb_output_append_velocity_dispersion("veldisp.txt");
	}
}
