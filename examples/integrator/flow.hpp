#ifndef INCLUDE_FLOW
#define INCLUDE_FLOW

#include "math.hpp"
#include <vector>
#include <algorithm>
#include <random>

static float PI = 3.1415926535897932384626433832795f;

// ============================================================
enum EIntegrator {
	Integrator_Euler, Integrator_RK2, Integrator_RK4
};

// ============================================================
// base class for fields (every field has a sample function)
template<typename vin, typename vout>
class field
{
public:
	virtual vout sample(const vin& pos) const = 0;
};

// ============================================================
// vector field is a map from vec -> vec
template<typename vec>
class vfield : public field<vec, vec>
{
public:
	// takes an Euler step
	vec stepEuler(const vec& pos, float dt) const {
		return pos + this->sample(pos) * dt;
	}

	// takes a 2nd-order Runge-Kutta step
	vec stepRK2(const vec& pos, float dt) const {
		vec k1 = this->sample(pos);
		vec k2 = this->sample(pos + k1*(dt/2));
		return pos + k2*dt;
	}

	// takes a 4th-order Runge-Kutta step
	vec stepRK4(const vec& pos, float dt) const {
		vec k1 = this->sample(pos);
		vec k2 = this->sample(pos + k1*(dt/2));
		vec k3 = this->sample(pos + k2*(dt/2));
		vec k4 = this->sample(pos + k3*dt);
		return pos + (k1/6 + k2/3 + k3/3 + k4/6)*dt;
	}

	// takes a step with the selected integrator
	vec step(const vec& pos, float dt, EIntegrator integrator) const {
		switch (integrator)
		{
		case Integrator_Euler:
			return stepEuler(pos, dt);
		case Integrator_RK2:
			return stepRK2(pos, dt);
		case Integrator_RK4:
		default:
			return stepRK4(pos, dt);
		}
	}
	
	// traces a tangent curve and returns all particle positions
	void tangentCurve(vec pos, float dt, float tau, EIntegrator integrator, std::vector<vec>& curve) const
	{
		curve.clear();
		curve.push_back(pos);
		for (float i = 0; i < tau; i += dt) {
			pos = step(pos, dt, integrator);
			curve.push_back(pos);
		}
	}
};

// ============================================================
// vector field in 2D
class vfield2d : public vfield<vec2f>
{
};

#endif // !INCLUDE_FLOW
