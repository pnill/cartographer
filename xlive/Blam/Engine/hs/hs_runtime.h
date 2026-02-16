#pragma once
#include "hs.h"

/* constants */

enum
{
	HS_THREAD_STACK_SIZE = 1024
};

/* enums */

enum e_hs_syntax_node_flags : uint16
{
	_hs_syntax_node_primitive_bit = 0,
	_hs_syntax_node_script_bit,
	_hs_syntax_node_variable_bit,
	_hs_syntax_node_permanent_bit,
	_hs_syntax_node_parameter_bit,
	_hs_syntax_node_line_number_bit,
	_hs_syntax_node_stripped_bit,
	NUMBER_OF_HS_SYNTAX_NODE_FLAGS
};

enum e_hs_thread_type
{
	_hs_thread_type_script = 0,
	_hs_thread_type_global_initialize,
	_hs_thread_type_runtime_evaluate,
	_hs_thread_type_command_script,
	_hs_thread_type_performance_script, // TODO: name
	NUMBER_OF_HS_THREAD_TYPES
};

/* structures */

struct hs_syntax_node
{
	int16 identifier;
	union
	{
		int16 script_index;
		int16 function_index;
		int16 constant_type;
	};

	e_hs_type type;
	/* e_hs_syntax_node_flags */ uint16 flags;
	int32 next_node_index;
	int32 source_offset;

	union
	{
		bool bool_value;
		real32 real_value;
		int16 short_value;
		int32 long_value;
		const char* string_value;
		string_id string_id_value;
		uint8 storage[4];
	};
};

struct hs_stack_frame
{
	struct hs_stack_frame* parent;
	int32 expression_index;
	int32* child_result;
	int16 size;
	// Disable warning that comaplins about variable length arrays
#pragma warning(push)
#pragma warning(disable : 4200)
	uint8 data[];	// Variable size
	// Disable warning that comaplins about variable length arrays
#pragma warning(pop)
};

struct hs_thread
{
	int16 identifier;
	uint8 type;
	uint8 flags;
	int32 script_index;
	int32 sleep_until;
	int32 latent_sleep_until;
	struct hs_stack_frame* stack;
	int32 result;
	uint8 stack_data[HS_THREAD_STACK_SIZE];
};

/* prototypes */

void hs_runtime_apply_patches(void);

data_array* hs_thread_data_get(void);

bool hs_runtime_initialized(void);

void hs_runtime_initialize(void);

void hs_runtime_update(void);

#ifdef HS_COMPILER_ENABLED

void hs_runtime_dirty(void);

void hs_runtime_require_gc(void);

bool hs_runtime_safe_to_gc(void);

#endif

void __cdecl hs_runtime_initialize_for_new_map(void);

void __cdecl hs_runtime_dispose_from_old_map(void);

struct hs_syntax_node* hs_syntax_get(datum index);

struct hs_thread* hs_thread_get(datum index);

int32 hs_thread_new(e_hs_thread_type type, int32 script_index);

void hs_thread_main_original(int32 thread_index);

void hs_thread_main(int32 thread_index);

void hs_thread_delete(int32 thread_index);

void hs_evaluate(int32 thread_index, int32 expression_index, int32* destination);

void __cdecl hs_return(int32 thread_index, int32 value);

int32* hs_macro_function_evaluate(int16 function_index, int32 thread_index, bool initialize);

int32 hs_runtime_evaluate(datum expression_index, bool display_expression_result);

bool hs_can_cast(int16 actual_type, int16 desired_type);

int32 hs_cast(int32 thread_index, int16 actual_type, int16 desired_type, int32 value);

void* hs_stack_allocate(int32 thread_index, int32 size);

void render_debug_scripting(void);

/* internal functions here */

void __cdecl hs_evaluate_wake(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_begin(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_begin_random(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_if(int16 function_index, int32 thread_index, bool initialize);

void hs_evaluate_set(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_logical(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_arithmetic(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_equality(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_inequality(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_sleep(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_sleep_forever(int16 function_index, int32 thread_index, bool initialize);

void __cdecl hs_evaluate_sleep_until(int16 function_index, int32 thread_index, bool initialize);

void hs_evaluate_inspect(int16 function_index, int32 thread_index, bool initialize);

void hs_evaluate_object_cast_up(int16 function_index, int32 thread_index, bool initialize);