//
//  DefragAllocator.h
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 21.11.2022.
//

#ifndef DefragAllocator_h
#define DefragAllocator_h

#include "Common.h"

#define DEFRAG_TYPE_SFXCACHE 0

typedef struct DefragMemoryblock_s
{
    u32 offset;
    u32 size;
} DefragMemoryblock_t;

#define DEFRAG_INFO_FLAG_CLEAR 0x01

typedef struct DefragInfo_s
{
    u16 gen;
    u8 type;
    u8 flags;
} DefragInfo_t;

typedef struct DefragLink_s
{
    u16 next;
    u16 prev;
} DefragLink_t;

typedef struct DefragLinkedList_s
{
    u16 head;
    u16 tail;
} DefragLinkedList_t;

void Defrag_OffsetPointer(void** ptr, ptrdiff_t offset);
int Defrag_CheckPointerOffset(void* ptr, u64 sizeInBytes, uintptr_t lowerBound, uintptr_t upperBound);

//void DefragRelocate_TYPE(void* ptr, uintptr_t lowerBound, uintptr_t upperBound, ptrdiff_t offset)
typedef void(*DefragRelocateFunc_t)(void*,uintptr_t,uintptr_t,ptrdiff_t);

#define DEFRAG_ALLOCATOR_INVALID_INDEX 0xFFFF
#define DEFRAG_ALLOCATOR_ALIGN 16
#define DEFRAG_ALLOCATOR_MAX_TYPES 8

// DefragAllocator - for allocation sizes 1-16 KB

typedef struct DefragAllocator_s
{
    DefragRelocateFunc_t relocateFuncs[DEFRAG_ALLOCATOR_MAX_TYPES];
    u8* data;
    DefragInfo_t* infos;
    DefragMemoryblock_t* blocks;
    // sharedLink can only be in one list at a time.
    // listUsed, listFree or listEmpty
    DefragLink_t* sharedLinks;
    DefragLinkedList_t listUsed;
    DefragLinkedList_t listFree;
    DefragLinkedList_t listEmpty;// memory blocks that have zero size.
    u32 reservedSize;
    u32 freeSize;
    u32 capacity;
    u32 currentAllocations;
    u32 maxAllocations;
} DefragAllocator_t;

void DefragAllocator_Init(DefragAllocator_t* da, u32 sizeInBytes, u32 maxAllocations);
void DefragAllocator_Free(DefragAllocator_t* da);
void DefragAllocator_Reset(DefragAllocator_t* da);
void DefragAllocator_SetRelocateFunction(DefragAllocator_t* da, u8 type, DefragRelocateFunc_t func);
void* DefragAllocator_GetPointer(DefragAllocator_t* da, u32 handle, u8 type);
u32 DefragAllocator_Allocate(DefragAllocator_t* da, u32 sizeInBytes, u8 type);
void DefragAllocator_Deallocate(DefragAllocator_t* da, u32 handle, u8 type);
void DefragAllocator_DefragRoutine(DefragAllocator_t* da);

typedef struct DefragCache_s
{
    DefragAllocator_t mem;
    DefragLink_t* links;
    DefragLinkedList_t list;
    u32 cachePurgeSize;
} DefragCache_t;

void DefragCache_Init(DefragCache_t* dc, u32 sizeInBytes, u32 cachePurgeInBytes, u32 maxAllocations);
void DefragCache_Free(DefragCache_t* dc);
void DefragCache_Reset(DefragCache_t* dc);
void* DefragCache_GetPointer(DefragCache_t* dc, u32 handle, u8 type);
u32 DefragCache_Allocate(DefragCache_t* dc, u32 sizeInBytes, u8 type, u32 maxTries);
void DefragCache_Deallocate(DefragCache_t* dc, u32 handle, u8 type);

#endif /* DefragAllocator_h */
