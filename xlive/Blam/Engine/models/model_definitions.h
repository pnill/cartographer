#pragma once

/* constants */

enum
{
    MODEL_DEFINITION_TAG = 'hlmt'
};

/* macros */

#define model_definition_get(index) ((struct s_model_definition*)tag_get(MODEL_DEFINITION_TAG, (index)))

/* globals */

extern const char* global_model_state_names[5];
