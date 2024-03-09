#include "Light.h"

Light::Light()
{
	enabled = false;
	color.set( 1.0f, 1.0f, 1.0f );
	intensity = 5000.0f;
	type = Light::POINT;
}