$input a_position

/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"

void main()
{
	gl_Position = mul(u_viewProj, vec4(a_position.xy, 0.0, 1.0));
}
