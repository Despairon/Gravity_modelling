#pragma once
#include "includes.h"

struct Pos
{
	float x, y, z;
	float dx,dy,dz;
};

class Gravity_center
{
private:
	float x,y,z;
	long double m;
public:
	float get_x(){return this->x;};
	float get_y(){return this->y;};
	float get_z(){return this->z;};
	long double get_m(){return this->m;};
	void set_m(long double mass){this->m += mass;};
	static bool gravity_on;
	static Gravity_center *g_c;
	Gravity_center(long double);
	~Gravity_center(){};
};

class physic_object
{
private:
	float x,y,z;
	long double m;
public:
	Pos pos;
	float get_x(){return this->x;}; 
	float get_y(){return this->y;};
	float get_z(){return this->z;};
	long double get_m(){return this->m;};
	long double R(Gravity_center); 
	long double F(Gravity_center); 
	long double a(Gravity_center); 
	static vector<physic_object*> objects;
	physic_object(float, float, float, long double);
	~physic_object(){};
};
