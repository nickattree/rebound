#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"

extern double opening_angle2;
void heartbeat(struct reb_simulation* const r);
double coefficient_of_restitution(const struct reb_simulation*r, double v){
	return 0.01;
}

int main(int argc, char* argv[]) {
	struct reb_simulation* r = reb_create_simulation();
	// Setup constants
	r->opening_angle2		= .5;					// This determines the precission of the tree code gravity calculation.
	r->integrator			= REB_INTEGRATOR_LEAPFROG;
	r->boundary			= REB_BOUNDARY_PERIODIC;
	r->gravity			= REB_GRAVITY_TREE;
	r->collision			= REB_COLLISION_TREE;
	r->dt 				= 0.1;	// s
	r->softening 			= 13;			// m
	r->coefficient_of_restitution 	= coefficient_of_restitution;
	r->G 				= 6.67428e-11;		// N / (1e-5 kg)^2 m^2

	double particle_density		= 900;			// kg/m^3
   	double xlen                	= 1000;			// Standard Deviation of normal distribution
    	double ylen             	= 1000;      		// Length of ring to be simulated
	double zlen			= 1000;
	double boxsize 			= 20000;		// m
	r->heartbeat			= heartbeat;	// function pointer for heartbeat

	reb_configure_box(r, boxsize, 1, 1, 1);
	r->nghostx = 0;
	r->nghosty = 0;
	r->nghostz = 0;


	// Add all ring paricles
	int i = 0;
	int number = 500;
	while(i<number){
		struct reb_particle pt = {0};
		pt.x 		= reb_random_normal(xlen*xlen);
		pt.y 		= reb_random_normal(ylen*ylen);
		pt.z 		= reb_random_normal(zlen*zlen);					// m
		pt.vx 		= 0;
		pt.vy 		= 0;
		pt.vz 		= 0;
		pt.r 		= 130.;						// m
		pt.m 		= particle_density*4./3.*M_PI*pt.r*pt.r*pt.r; 	// kg
		pt.id		= i;
		reb_add(r, pt);
		i ++;
	}
	reb_integrate(r, 24000);
	reb_output_binary(r, "position.bin");
	reb_free_simulation(r);
	r = NULL;
}

void heartbeat(struct reb_simulation* const r){
	if (reb_output_check(r, 1.)){
		reb_output_timing(r, 0);
		//reb_output_append_velocity_dispersion("veldisp.txt");
	}
	
}
