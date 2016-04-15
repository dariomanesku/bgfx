/*
 * Copyright 2010-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef INPUT_H_HEADER_GUARD
#define INPUT_H_HEADER_GUARD

#include "entry.h"

typedef void (*InputBindingFn)(const void* _userData);

struct InputBinding
{
	entry::Key::Enum m_key;
	uint8_t m_modifiers;
	uint8_t m_flags;
	InputBindingFn m_fn;
	const void* m_userData;
};

#define INPUT_BINDING_END { entry::Key::None, entry::Modifier::None, 0, NULL, NULL }

///
void inputInit();

///
void inputShutdown();

///
void inputAddBindings(const char* _name, const InputBinding* _bindings);

///
void inputRemoveBindings(const char* _name);

///
void inputProcess();

///
void inputSetKeyState(entry::Key::Enum  _key, uint8_t _modifiers, bool _down);

///
bool inputGetKeyState(entry::Key::Enum _key, uint8_t* _modifiers = NULL);

///
uint8_t inputGetModifiersState();

/// Adds single UTF-8 encoded character into input buffer.
void inputChar(uint8_t _len, const uint8_t _char[4]);

/// Returns single UTF-8 encoded character from input buffer.
const uint8_t* inputGetChar();

/// Flush internal input buffer.
void inputCharFlush();

///
void inputSetMouseResolution(uint16_t _width, uint16_t _height);

///
void inputSetMousePos(int32_t _mx, int32_t _my, int32_t _mz);

///
void inputSetMouseButtonState(entry::MouseButton::Enum _button, uint8_t _state);

///
void inputGetMouseButtonState(uint8_t _mb[entry::MouseButton::Count]);

///
void inputSetMouseLock(bool _lock);

///
void inputSetMouseMotion(entry::MouseMotion::Enum _mm);

///
void inputGetMouse(float _mouse[3]);

///
void inputGetMouseAbsolute(int32_t _mouse[3]);

///
bool inputIsMouseLocked();

///
void inputSetGamepadAxis(entry::GamepadHandle _handle, entry::GamepadAxis::Enum _axis, int32_t _value);

///
int32_t inputGetGamepadAxis(entry::GamepadHandle _handle, entry::GamepadAxis::Enum _axis);

///
void systemSetWindowMousePos(int32_t _mx, int32_t _my);

///
void systemShowMouseCursor(bool _show);

#endif // INPUT_H_HEADER_GUARD
