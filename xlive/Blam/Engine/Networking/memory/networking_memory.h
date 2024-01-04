#pragma once

uint8* network_heap_allocate_block(uint32 size);

void network_heap_free_block(uint8* block);

void network_heap_verify_block(uint8* block);