/**
 * A.S. This is my planetesimal disk with special integrator for close encounters.
 *      Particle id's: 0 = star, 1 = massive body, 2 = planetesimal, 3 = CLOSE ENCOUNTER
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "rebound.h"

void heartbeat(struct reb_simulation* r);
double E0;

int main(int argc, char* argv[]){
    struct reb_simulation* r = reb_create_simulation();

    int N_planetesimals = 100;
    double planetesimal_mass = 3e-8;
    double amin = 0.45, amax = 0.75;        //for planetesimal disk
    double powerlaw = 0.5;
    
	//Simulation Setup
	r->integrator	= REB_INTEGRATOR_HYBARID;
	//r->integrator	= REB_INTEGRATOR_IAS15;
	//r->integrator	= REB_INTEGRATOR_WHFAST;
    r->ri_hybarid.switch_ratio = 3.;
    r->passive_influence = 1;
	r->heartbeat	= heartbeat;
    r->dt = 0.01;
    
	// Initial conditions
	struct reb_particle star = {0};
	star.m 		= 1;
	star.r		= 0.005;        // Radius of particle is in AU!
	reb_add(r, star);
   
    srand(11);
    
    //planet 1
    {
        double a=0.5, m=5e-4, e=0, inc = reb_random_normal(0.00001);
        struct reb_particle p1 = {0};
        p1 = reb_tools_orbit_to_particle(r->G, star, m, a, e, inc, 0, 0, 0);
        p1.r = 1.6e-4;              //radius of particle is in AU!
        reb_add(r, p1);
    }
    
    //planet 2
    {
        double a=0.7, m=5e-4, e=0.01, inc=reb_random_normal(0.00001);
        struct reb_particle p2 = {0};
        p2 = reb_tools_orbit_to_particle(r->G, star, m, a, e, inc, 0, 0, 0);
        p2.r = 1.6e-4;
        reb_add(r, p2);
    }
    
    r->N_active = r->N;
    reb_move_to_com(r);
    
    //planetesimals
    while(r->N<N_planetesimals + r->N_active){
		struct reb_particle pt = {0};
		double a	= reb_random_powerlaw(amin,amax,powerlaw);
        double phi 	= reb_random_uniform(0,2.*M_PI);
        double inc = reb_random_normal(0.0001);
        double Omega = reb_random_uniform(0,2.*M_PI);
        double apsis = reb_random_uniform(0,2.*M_PI);
        pt = reb_tools_orbit_to_particle(r->G, star, planetesimal_mass, a, 0., inc, Omega, apsis,phi);
		pt.r 		= 4e-5;
		reb_add(r, pt);
    }
   
    E0 = reb_tools_energy(r);
    //time_t t_ini = time(NULL);
    //struct tm *tmp = gmtime(&t_ini);
    
    //Integrate!
    reb_integrate(r, 100.);
    
    //time_t t_fini = time(NULL);
    //struct tm *tmp2 = gmtime(&t_fini);
    //double time = t_fini - t_ini;
    //printf("\nSimulation complete. Elapsed simulation time is %.2f s, \n\n",time);
    
}

void heartbeat(struct reb_simulation* r){
    double E = reb_tools_energy(r) + r->ri_hybarid.dE_offset;
    double dE = fabs((E-E0)/E0);
    reb_output_timing(r, 0);
    printf("    dE=%e",dE);
}
