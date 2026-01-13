#pragma once

#include "havok/hkbase/memoryclasses/hkMemoryClasses.h"
#include "havok/hkbase/types/hkBaseTypes.h"

class hkMemory
{
public:

	hkMemory();

	virtual void* allocate(int nbytes, HK_MEMORY_CLASS cl) = 0;

	virtual void  deallocate(void* ptr) = 0;

	virtual void* alignedAllocate(int alignment, int nbytes, HK_MEMORY_CLASS cl) = 0;

	virtual void  alignedDeallocate(void* ptr) = 0;

	virtual void* allocateChunk(int nbytes, HK_MEMORY_CLASS cl) = 0;

	virtual void deallocateChunk(void*, int nbytes, HK_MEMORY_CLASS) = 0;

	HK_FORCE_INLINE void* allocateStack(int nbytes);

	HK_FORCE_INLINE void deallocateStack(void* p, int nbytes);

	void setStackArea(void* buf, int nbytes);

	virtual void returnStackAreaToDefault() {}

	virtual void printStatistics(class hkOstream* c) = 0;

	virtual void getStatSynopsis(class hkMemoryStatistics* in) = 0;

	static inline hkMemory& HK_CALL getInstance();

	static void HK_CALL replaceInstance(hkMemory* m);

	HK_FORCE_INLINE void addReference();

	HK_FORCE_INLINE void removeReference();

protected:

	virtual ~hkMemory() {}

	virtual void* onStackOverflow(int nbytes);

	virtual void onStackUnderflow(void* p, int nbytes);

	struct Stack
	{
		Stack()
			: m_current(HK_NULL),
			m_freebytes(0),
			m_prev(HK_NULL),
			m_base((char*)(-1))
		{
		}
		char* m_current;
		int m_freebytes;
		Stack* m_prev;
		char* m_base;
	};
	static hkMemory* s_instance;
	int m_referenceCount;
	Stack m_stack;
};
