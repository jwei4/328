#include <math.h>
#include "Vec3.h"

Vec3::Vec3(float x, float y, float z){
	f[0] =x;
	f[1] =y;
	f[2] =z;
}

float Vec3::length() {
	return sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
}

Vec3 Vec3::normalized() {
	float l = length();
	return Vec3(f[0]/l,f[1]/l,f[2]/l);
}

//Vec3 Operations

void Vec3::operator+= (const Vec3 &v) {
	f[0]+=v.f[0];
	f[1]+=v.f[1];
	f[2]+=v.f[2];
}

Vec3 Vec3::operator/ (const float &a) {
	return Vec3(f[0]/a,f[1]/a,f[2]/a);
}

Vec3 Vec3::operator- (const Vec3 &v) {
	return Vec3(f[0]-v.f[0],f[1]-v.f[1],f[2]-v.f[2]);
}

Vec3 Vec3::operator+ (const Vec3 &v) {
	return Vec3(f[0]+v.f[0],f[1]+v.f[1],f[2]+v.f[2]);
}

Vec3 Vec3::operator* (const float &a) {
	return Vec3(f[0]*a,f[1]*a,f[2]*a);
}

Vec3 Vec3::operator-() {
	return Vec3(-f[0],-f[1],-f[2]);
}

//cross product
Vec3 Vec3::cross(const Vec3 &v) {
	return Vec3(f[1]*v.f[2] - f[2]*v.f[1], f[2]*v.f[0] - f[0]*v.f[2], f[0]*v.f[1] - f[1]*v.f[0]);
}

//dot product
float Vec3::dot(const Vec3 &v) {
	return f[0]*v.f[0] + f[1]*v.f[1] + f[2]*v.f[2];
}

float Vec3::distance(const Vec3 &v){
	//std::cout << sqrt( pow((v.f[0]-f[0]), 2) + pow((v.f[1]-f[1]), 2) + pow((v.f[2]-f[2]), 2)) << "\n";
	return sqrt( pow((v.f[0]-f[0]), 2) + pow((v.f[1]-f[1]), 2) + pow((v.f[2]-f[2]), 2));
}