#pragma once

// useful to remove all pointer extends from a base type

// while this is not in the standard libary
// include it in the namespace for convenience purposes
// not entirely sure why this wasn't added because it can be very useful
_STD_BEGIN

template <class _Ty>
struct remove_all_pointers {
	using type = _Ty;
};

template <class _Ty>
struct remove_all_pointers <_Ty*> {
	using type = typename remove_all_pointers<_Ty>::type;
};

template <class _Ty>
struct remove_all_pointers <_Ty* const> {
	using type = typename remove_all_pointers<_Ty>::type;
};

template <class _Ty>
struct remove_all_pointers <_Ty* volatile> {
	using type = typename remove_all_pointers<_Ty>::type;
};

template <class _Ty>
struct remove_all_pointers <_Ty* const volatile> {
	using type = typename remove_all_pointers<_Ty>::type;
};

template <class _Ty>
using remove_all_pointer_t = typename remove_all_pointers<_Ty>::type;

_STD_END