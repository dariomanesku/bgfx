/*
 * Copyright 2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

namespace entry
{
	typedef void (*DropCallbackFn)(const char* _path);
	void setDropCallback(DropCallbackFn _fn);
} // namespace entry
