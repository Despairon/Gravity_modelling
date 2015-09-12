#include "Physic_object.h"
vector<physic_object*> physic_object::objects;
bool Gravity_center::gravity_on = false;
Gravity_center *Gravity_center::g_c;
physic_object::physic_object(float x = 0, float y = 0, float z = 0, long double m = 0)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->m = m;
	this->pos.x = 0.0f;
	this->pos.y = 0.0f;
	this->pos.z = 0.0f;
	this->pos.dx = 0.0f;
	this->pos.dy = 0.0f;
	this->pos.dz = 0.0f;
	objects.push_back(this);
}

long double physic_object::R(Gravity_center g_c)
{
	long double xb_minus_xa = g_c.get_x()-this->get_x();
	long double yb_minus_ya = g_c.get_y()-this->get_y();
	long double zb_minus_za = g_c.get_z()-this->get_z();
	long double dst = sqrt(pow(xb_minus_xa,2)+pow(yb_minus_ya,2)+pow(zb_minus_za,2));
	return dst;
}

long double physic_object::F(Gravity_center g_c)
{
	static const long double G = 0.0000000000667384; //gravity constant
	long double MassMult = this->get_m()*g_c.get_m();
	long double f = G*( MassMult/pow(this->R(g_c),2) );
	return f;
}

long double physic_object::a(Gravity_center g_c)
{
	//static const long double G = 0.0000000000667384;
	long double a = F(g_c)/get_m(); //a = (G*g_pt.get_m())/pow(this->R(g_pt),2);
	return a; 
}
Gravity_center::Gravity_center(long double m = 0)
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->m = m;
}