#pragma once
#include "tag_files/tag_reference.h"

// max count: MAXIMUM_DIALOGUE_VARIANTS_PER_UNIT 16
struct dialogue_variant_definition
{
    int16 variant_number;   // variant number to use this dialogue with (must match the suffix in the permutations on the unit's model)
    int16 pad;
    tag_reference dialogue; // udlg
};
TAG_BLOCK_SIZE_ASSERT(dialogue_variant_definition, 12);
