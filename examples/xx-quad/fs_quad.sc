$input

/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"

void main()
{
	vec3 color = vec3(1.0, 0.0, 0.0);

	gl_FragColor.xyz = color;
	gl_FragColor.w = 1.0;
}
