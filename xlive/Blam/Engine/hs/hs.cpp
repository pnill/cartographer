#include "stdafx.h"
#include "hs.h"

#include "Blam/Engine/objects/objects.h"


int hs_object_get_markers_by_string_id(datum object_datum, string_id marker_id, void* data, int16 count)
{
	return object_get_marker_by_string_id(object_datum, marker_id, data, count, false);
}