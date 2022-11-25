#pragma once

#define ENTITY_INDEX_NONE ((unsigned int)(0xFFFFFFFF))

#define ENTITY_IS_NONE(_entity_index) ((_entity_index) == DATUM_INDEX_NONE)
#define ENTITY_INDEX_TO_ABSOLUTE_INDEX(_entity_index)  ((_entity_index) & 0x3FF)
#define ENTITY_INDEX_TO_SEED(_entity_index) ((_entity_index) >> 0x1C)

