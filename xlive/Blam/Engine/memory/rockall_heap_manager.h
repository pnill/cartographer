#pragma once

struct c_fixed_memory_rockall_frontend_vtbl
{
    bool(__thiscall* Deletet)(int a1, int a2, int a3);
    bool(__thiscall* Details)(int a1, uint8* block, int32* out_size);
    bool(__thiscall* KnownArea)(int a1, int a2);
    bool(__thiscall* MultipleDelete)(int a1, int a2, int a3, int a4);
    bool(__thiscall* MultipleNew)(int a1, int a2, int a3, int a4, int a5, int a6, unsigned __int8 a7);
    int(__thiscall* New)(int a1, int a2, int a3, unsigned __int8 a4);
    void(__thiscall* Resizel)(int* a1, int a2, int a3, int a4, int a5, unsigned __int8 a6, unsigned __int8 a7);
    void(__thiscall* Verify_void__PTR_int__PTR)(int a1, int a2, int a3);
    bool(__thiscall* DeleteAll)(int a1, unsigned __int8 a2);
    void(__thiscall* LockAll)(int a1);
    int(__thiscall* Truncate)(int a1, int a2);
    void(__thiscall* UnlockAll)(int a1);
    bool(__thiscall* Walk)(int a1, BYTE* a2, int a3, int a4);
    int(__thiscall* _vector_deleting_destructor)(int a1, char a2);
    char* (__thiscall* Exception)();
    int(__thiscall* SpecialNew)(int a1, int a2);
};

class c_fixed_memory_rockall_frontend
{
public:
    virtual bool Deletet(int a2, int a3) = 0;
    virtual bool Details(const uint8* memory_block, int32* out_size) = 0;
};
