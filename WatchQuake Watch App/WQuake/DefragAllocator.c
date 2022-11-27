//
//  DefragAllocator.c
//  WatchQuake Watch App
//
//  Created by ByteOverlord on 21.11.2022.
//

#include "DefragAllocator.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

void Defrag_OffsetPointer(void** ptr, ptrdiff_t offset)
{
#ifdef DEBUG
    assert(ptr != NULL);
#endif
    // don't offset NULL pointers!
    // if NULL pointer gets offset, how do you know it is supposed to be NULL?
    if (*ptr != NULL)
    {
        *ptr = (void*)(((uintptr_t)(*ptr)) + offset);
    }
}

int Defrag_CheckPointerOffset(void* ptr, u64 sizeInBytes, uintptr_t lowerBound, uintptr_t upperBound)
{
    if (ptr == NULL)
    {
        return 1;
    }
    return (uintptr_t)(ptr) >= lowerBound && (uintptr_t)((ptr) + sizeInBytes) <= upperBound;
}

#define DEFRAG_LIST_AT_HEAD 0
#define DEFRAG_LIST_AT_TAIL 1
#define DEFRAG_LIST_AFTER 0
#define DEFRAG_LIST_BEFORE 1

void DefragLinkedlist_Insert(DefragLink_t* links, DefragLinkedList_t* list, int atTail, u32 newLink)
{
    DefragLink_t* link = &links[newLink];
    link->next = DEFRAG_ALLOCATOR_INVALID_INDEX;
    link->prev = DEFRAG_ALLOCATOR_INVALID_INDEX;

    if (list->head != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        if (atTail)
        {
            links[list->tail].next = newLink;
            link->prev = list->tail;
            list->tail = newLink;
        }
        else
        {
            link->next = list->head;
            links[list->head].prev = newLink;
            list->head = newLink;
        }
    }
    else
    {
        list->head = list->tail = newLink;
    }
}

u32 DefragLinkedlist_Remove(DefragLink_t* links, DefragLinkedList_t* list, int atTail)
{
    if (list->head != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        if (atTail)
        {
            u32 n = list->tail;
            list->tail = links[list->tail].prev;
            if (list->tail == DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                list->head = DEFRAG_ALLOCATOR_INVALID_INDEX;
            }
            else
            {
                links[list->tail].next = DEFRAG_ALLOCATOR_INVALID_INDEX;
            }
            return n;
        }
        else
        {
            u32 n = list->head;
            list->head = links[list->head].next;
            if (list->head == DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                list->tail = DEFRAG_ALLOCATOR_INVALID_INDEX;
            }
            else
            {
                links[list->head].prev = DEFRAG_ALLOCATOR_INVALID_INDEX;
            }
            return n;
        }
    }
    else
    {
        assert(0);
    }
    return DEFRAG_ALLOCATOR_INVALID_INDEX;
}

void DefragLinkedlist_InsertFrom(DefragLink_t* links, DefragLinkedList_t* list, int before, u32 index, u32 newLink)
{
    if (index != DEFRAG_ALLOCATOR_INVALID_INDEX && newLink != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        if (before)
        {
            if (index == list->head)
            {
                DefragLinkedlist_Insert(links,list,DEFRAG_LIST_AT_HEAD,newLink);
            }
            else
            {
                DefragLink_t* next = &links[index];
                if (next->prev == DEFRAG_ALLOCATOR_INVALID_INDEX)
                {
                    assert(0);
                }
                DefragLink_t* prev = &links[next->prev];
                DefragLink_t* link = &links[newLink];
                link->next = prev->next;
                link->prev = next->prev;
                prev->next = newLink;
                next->prev = newLink;
            }
        }
        else
        {
            if (index == list->tail)
            {
                DefragLinkedlist_Insert(links,list,DEFRAG_LIST_AT_TAIL,newLink);
            }
            else
            {
                DefragLink_t* prev = &links[index];
                if (prev->next == DEFRAG_ALLOCATOR_INVALID_INDEX)
                {
                    assert(0);
                }
                DefragLink_t* next = &links[prev->next];
                DefragLink_t* link = &links[newLink];
                link->next = prev->next;
                link->prev = next->prev;
                prev->next = newLink;
                next->prev = newLink;
            }
        }
    }
    else
    {
        assert(0);
    }
}

void DefragLinkedlist_RemoveFrom(DefragLink_t* links, DefragLinkedList_t* list, u32 index)
{
    if (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        if (list->head == index || list->tail == index)
        {
            DefragLinkedlist_Remove(links,list,list->head == index ? DEFRAG_LIST_AT_HEAD : DEFRAG_LIST_AT_TAIL);
        }
        else
        {
            DefragLink_t* link = &links[index];
            if (link->prev != DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                DefragLink_t* prev = &links[link->prev];
                prev->next = link->next;
            }
            if (link->next != DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                DefragLink_t* next = &links[link->next];
                next->prev = link->prev;
            }
        }
    }
    else
    {
        assert(0);
    }
}

void DefragMemoryblock_Swap(DefragMemoryblock_t* b0, DefragMemoryblock_t* b1)
{
    //printf("%u %u\n",b0->offset,b0->size);
    //printf("%u %u\n",b1->offset,b1->size);
    b1->offset = b0->offset;
    b0->offset += (i32)(b1->size);
}

void DefragAllocator_Reset(DefragAllocator_t* da);

void DefragAllocator_Init(DefragAllocator_t* da, u32 sizeInBytes, u32 maxAllocations)
{
    assert(maxAllocations < DEFRAG_ALLOCATOR_INVALID_INDEX);
    da->data = AlignedMalloc(sizeInBytes,DEFRAG_ALLOCATOR_ALIGN);
    da->infos = AlignedMalloc(maxAllocations * sizeof(DefragInfo_t),4);
    da->sharedLinks = AlignedMalloc(maxAllocations * sizeof(DefragLink_t),4);
    da->blocks = AlignedMalloc(maxAllocations * sizeof(DefragMemoryblock_t),4);
    da->capacity = sizeInBytes;
    da->maxAllocations = maxAllocations;
    memset(da->relocateFuncs,0,sizeof(DefragRelocateFunc_t)*DEFRAG_ALLOCATOR_MAX_TYPES);
    DefragAllocator_Reset(da);
}

void DefragAllocator_Free(DefragAllocator_t* da)
{
    AlignedFree(da->blocks);
    AlignedFree(da->sharedLinks);
    AlignedFree(da->infos);
    AlignedFree(da->data);
}

void DefragAllocator_Reset(DefragAllocator_t* da)
{
    memset(da->data,0,da->capacity);

    da->listUsed.head = DEFRAG_ALLOCATOR_INVALID_INDEX;
    da->listFree.head = 0;
    da->listEmpty.head = 1;
    //
    da->listUsed.tail = DEFRAG_ALLOCATOR_INVALID_INDEX;
    da->listFree.tail = 0;
    //
    da->reservedSize = 0;
    da->freeSize = da->capacity;
    da->currentAllocations = 0;
    u32 last = da->maxAllocations-1;
    for (u32 i=0; i<da->maxAllocations; i++)
    {
        da->infos[i].gen = 0;
        da->infos[i].type = 0xFF;
        da->infos[i].flags = DEFRAG_INFO_FLAG_CLEAR;
    }
    // emptylist
    for (u32 i=1; i<last; i++)
    {
        da->blocks[i].offset = 0;
        da->blocks[i].size = 0;
        da->sharedLinks[i].next = i+1;
        da->sharedLinks[i].prev = i-1;
    }
    da->listEmpty.tail = last;
    // emptylist
    da->sharedLinks[1].prev = DEFRAG_ALLOCATOR_INVALID_INDEX;
    // emptylist
    da->blocks[last].offset = 0;
    da->blocks[last].size = 0;
    da->sharedLinks[last].next = DEFRAG_ALLOCATOR_INVALID_INDEX;
    da->sharedLinks[last].prev = last-1;
    // freelist
    da->blocks[0].offset = 0;
    da->blocks[0].size = da->capacity;
    da->sharedLinks[0].next = DEFRAG_ALLOCATOR_INVALID_INDEX;
    da->sharedLinks[0].prev = DEFRAG_ALLOCATOR_INVALID_INDEX;
}

void DefragAllocator_SetRelocateFunction(DefragAllocator_t* da, u8 type, DefragRelocateFunc_t func)
{
    assert(type < DEFRAG_ALLOCATOR_MAX_TYPES);
    assert(da->relocateFuncs[type] == NULL);
    da->relocateFuncs[type] = func;
}

void* DefragAllocator_GetPointer(DefragAllocator_t* da, u32 handle, u8 type)
{
    if (handle == 0)
    {
        return NULL;
    }
    u32 index = handle & 0xFFFF;
    u32 gen = (handle >> 16) & 0xFFFF;
    if (index < da->maxAllocations)
    {
        DefragInfo_t* info = &da->infos[index];
        if (info->type == type && info->gen == gen)
        {
            DefragMemoryblock_t* mem = &da->blocks[index];
            return (da->data + mem->offset);
        }
    }
    return NULL;
}

int DefragAllocator_GetInsertDirection(DefragAllocator_t* da, DefragLinkedList_t* list, DefragMemoryblock_t* mem)
{
    int insertFrom = DEFRAG_LIST_AT_HEAD;
    if (list->head != list->tail)
    {
        insertFrom = mem->offset > ((da->blocks[list->head].offset + da->blocks[list->tail].offset + da->blocks[list->tail].size) >> 1) ? DEFRAG_LIST_AT_TAIL : DEFRAG_LIST_AT_HEAD;
    }
    return insertFrom;
}

void DefragAllocator_AddressOrderedInsert(DefragAllocator_t* da, DefragLinkedList_t* list, u32 newNode)
{
    if (list->head != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragMemoryblock_t* mem = &da->blocks[newNode];
        int atTail = DefragAllocator_GetInsertDirection(da,list,mem);
        if (atTail)
        {
            u32 index = list->tail;
            while (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                DefragLink_t* prevL = &da->sharedLinks[index];
                DefragMemoryblock_t* prev = &da->blocks[index];
                if (mem->offset > prev->offset)
                {
                    DefragLinkedlist_InsertFrom(da->sharedLinks,list,DEFRAG_LIST_AFTER,index,newNode);
                    return;
                }
                else if (index == list->head)
                {
                    DefragLinkedlist_Insert(da->sharedLinks,list,DEFRAG_LIST_AT_HEAD,newNode);
                    return;
                }
                index = prevL->prev;
            }
        }
        else
        {
            u32 index = list->head;
            while (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
            {
                DefragLink_t* nextL = &da->sharedLinks[index];
                DefragMemoryblock_t* next = &da->blocks[index];
                if (mem->offset < next->offset)
                {
                    DefragLinkedlist_InsertFrom(da->sharedLinks,list,DEFRAG_LIST_BEFORE,index,newNode);
                    return;
                }
                else if (index == list->tail)
                {
                    DefragLinkedlist_Insert(da->sharedLinks,list,DEFRAG_LIST_AT_TAIL,newNode);
                    return;
                }
                index = nextL->next;
            }
        }
    }
    else
    {
        DefragLinkedlist_Insert(da->sharedLinks,list,DEFRAG_LIST_AT_HEAD,newNode);
    }
}

void DefragAllocator_MoveBlock(DefragAllocator_t* da, u32 b0, u32 b1)
{
    DefragMemoryblock_t* a = &da->blocks[b0];
    DefragMemoryblock_t* b = &da->blocks[b1];
    assert((a->offset + (u32)(a->size)) == b->offset);
    i32 offset = (i32)(a->offset) - (i32)(b->offset);
    assert(offset < 0);
    memcpy(da->data + a->offset,da->data + b->offset,b->size);
    DefragMemoryblock_Swap(a,b);
    // todo
    //
    // clear free memory in a smarter way
    //
    // clear memory when block is in "high" address with other free blocks
    // set flag when block is cleared
    // when allocating memory check if block is already cleared
    //
    // do we need to clear all memory blocks?
    // add a parameter to DefragAllocator_Allocate telling if block needs to be cleared
    //
    DefragInfo_t* aInfo = &da->infos[b0];
    aInfo->flags &= ~DEFRAG_INFO_FLAG_CLEAR;
    //memset(da->data + a->offset,0,a->size);
    DefragInfo_t* bInfo = &da->infos[b1];
    if (bInfo->type < DEFRAG_ALLOCATOR_MAX_TYPES && da->relocateFuncs[bInfo->type])
    {
        u8* data = da->data;
        (*da->relocateFuncs[bInfo->type])(data + b->offset,(uintptr_t)(data) + b->offset,(uintptr_t)(data) + b->offset + b->size,offset);
    }
    else
    {
        assert(0);
    }
}

void DefragAllocator_MergeBlocks(DefragAllocator_t* da, u32 b0, u32 b1)
{
    DefragMemoryblock_t* block = &da->blocks[b0];
    DefragMemoryblock_t* next = &da->blocks[b1];
    if (da->listFree.tail == b1 || (da->infos[b1].flags & DEFRAG_INFO_FLAG_CLEAR))
    {
        if (!(da->infos[b0].flags & DEFRAG_INFO_FLAG_CLEAR))
        {
            memset(da->data + block->offset,0,block->size);
            da->infos[b0].flags |= DEFRAG_INFO_FLAG_CLEAR;
        }
        if (!(da->infos[b1].flags & DEFRAG_INFO_FLAG_CLEAR))
        {
            memset(da->data + next->offset,0,next->size);
            da->infos[b1].flags |= DEFRAG_INFO_FLAG_CLEAR;
        }
    }
    block->size += next->size;
    DefragLinkedlist_RemoveFrom(da->sharedLinks,&da->listFree,b1);
    DefragLinkedlist_Insert(da->sharedLinks,&da->listEmpty,DEFRAG_LIST_AT_TAIL,b1);
}

void DefragAllocator_TrySplitBlock(DefragAllocator_t* da, u32 blockIndex, u32 size)
{
    DefragMemoryblock_t* block = &da->blocks[blockIndex];
    u32 nextAddr = block->offset + size;
    u32 align = nextAddr % DEFRAG_ALLOCATOR_ALIGN;
    if (align != 0)
    {
        u32 alignFix = DEFRAG_ALLOCATOR_ALIGN - align;
        nextAddr += alignFix;
        size += alignFix;
        assert((nextAddr % DEFRAG_ALLOCATOR_ALIGN) == 0);
        if (block->size <= size)// alignment makes block larger, no extra space left
        {
            return;
        }
    }
    u32 nextIndex = DefragLinkedlist_Remove(da->sharedLinks,&da->listEmpty,DEFRAG_LIST_AT_HEAD);

    DefragMemoryblock_t* next = &da->blocks[nextIndex];
    assert(nextIndex != blockIndex);

    //printf("split[%u] offset %u size %u to size %u\n",blockIndex,block->offset,block->size,size);

    next->offset = nextAddr;
    next->size = block->size - size;

    block->size = size;

    da->infos[nextIndex].flags = da->infos[blockIndex].flags;

    assert(block->size > 0);
    assert(next->size > 0);
    //printf("b[%u] offset %u size %u\n",blockIndex,block->offset,block->size);
    //printf("b[%u] offset %u size %u\n",nextIndex,next->offset,next->size);

    DefragAllocator_AddressOrderedInsert(da,&da->listFree,nextIndex);
}

u32 DefragAllocator_FindFreeBlock(DefragAllocator_t* da, u32 size)
{
    u32 index = da->listFree.head;
    while (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragLink_t* memLink = &da->sharedLinks[index];
        DefragMemoryblock_t* mem = &da->blocks[index];
        if (mem->size >= size)
        {
            DefragLinkedlist_RemoveFrom(da->sharedLinks,&da->listFree,index);
            return index;
        }
        index = memLink->next;
    }
    return DEFRAG_ALLOCATOR_INVALID_INDEX;
}

u32 DefragAllocator_Allocate(DefragAllocator_t* da, u32 sizeInBytes, u8 type)
{
    if (da->currentAllocations >= da->maxAllocations)
    {
        return 0;
    }
    u32 blockIndex = DefragAllocator_FindFreeBlock(da,sizeInBytes);
    if (blockIndex != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragInfo_t* info = &da->infos[blockIndex];
        DefragMemoryblock_t* mem = &da->blocks[blockIndex];
        if (mem->size > sizeInBytes)
        {
            DefragAllocator_TrySplitBlock(da,blockIndex,sizeInBytes);
        }
        if (!(info->flags & DEFRAG_INFO_FLAG_CLEAR))
        {
            memset(da->data + mem->offset,0,mem->size);
        }
        info->flags &= ~DEFRAG_INFO_FLAG_CLEAR;
        info->type = type;
        if (info->gen == 0)
        {
            info->gen = 1;
        }
        //printf("block[%u] offset %u size %u\n",blockIndex,mem->offset,mem->size);
        u32 handle = blockIndex;
        handle |= ((u32)info->gen) << 16;
        DefragAllocator_AddressOrderedInsert(da,&da->listUsed,blockIndex);
        da->reservedSize += mem->size;
        da->freeSize -= mem->size;
        ++da->currentAllocations;
        return handle;
    }
    return 0;
}

void DefragAllocator_Deallocate(DefragAllocator_t* da, u32 handle, u8 type)
{
    if (handle)
    {
        u32 index = handle & 0xFFFF;
        u32 gen = (handle >> 16) & 0xFFFF;
        DefragInfo_t* info = &da->infos[index];
        if (info->type == type && info->gen == gen)
        {
            DefragMemoryblock_t* mem = &da->blocks[index];
            da->reservedSize -= mem->size;
            da->freeSize += mem->size;

            info->type = 0xFF;
            info->gen++;
            DefragLinkedlist_RemoveFrom(da->sharedLinks,&da->listUsed,index);
            DefragAllocator_AddressOrderedInsert(da,&da->listFree,index);
            --da->currentAllocations;
        }
        else
        {
            assert(0);
        }
    }
}

u32 DefragAllocator_DeallocateAndReturnIndex(DefragAllocator_t* da, u32 handle, u8 type)
{
    if (handle)
    {
        u32 index = handle & 0xFFFF;
        u32 gen = (handle >> 16) & 0xFFFF;
        DefragInfo_t* info = &da->infos[index];
        if (info->type == type && info->gen == gen)
        {
            DefragMemoryblock_t* mem = &da->blocks[index];
            da->reservedSize -= mem->size;
            da->freeSize += mem->size;

            info->type = 0xFF;
            info->gen++;
            DefragLinkedlist_RemoveFrom(da->sharedLinks,&da->listUsed,index);
            DefragAllocator_AddressOrderedInsert(da,&da->listFree,index);
            --da->currentAllocations;
            return index;
        }
        else
        {
            assert(0);
        }
    }
    return DEFRAG_ALLOCATOR_INVALID_INDEX;
}

int DefragAllocator_HasMoreThanOneFreeBlock(DefragAllocator_t* da)
{
    // if list.head == list.tail, list is empty or has one item
    return da->listFree.head != da->listFree.tail;
}

u32 DefragAllocator_MoveLowestFreeBlock(DefragAllocator_t* da, u32* maxBytesToMove)
{
    u32 resIndex = da->listFree.head;
    if (resIndex == DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        return DEFRAG_ALLOCATOR_INVALID_INDEX;
    }
    DefragMemoryblock_t* b0 = &da->blocks[resIndex];
    u32 endAddr = b0->offset + b0->size;
    u32 usedIndex = da->listUsed.head;
    int moved = 0;
    while (usedIndex != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragLink_t* b1Link = &da->sharedLinks[usedIndex];
        DefragMemoryblock_t* b1 = &da->blocks[usedIndex];
        if (b1->offset == endAddr)
        {
            moved = 1;
            DefragAllocator_MoveBlock(da,resIndex,usedIndex);
            endAddr = b0->offset + b0->size;
            //printf("defrag block moved\n");
            if ((*maxBytesToMove) > b1->size)
            {
                *maxBytesToMove = (*maxBytesToMove) - b1->size;
            }
            else
            {
                *maxBytesToMove = 0;
                break;
            }
        }
        else if (moved)// previous block was used, next used block is after a free block
        {
            break;
        }
        usedIndex = b1Link->next;
    }
    return resIndex;
}

void DefragAllocator_TryMergeFreeBlock(DefragAllocator_t* da, u32 index)
{
    DefragLink_t* b0Link = &da->sharedLinks[index];
    DefragMemoryblock_t* b0 = &da->blocks[index];
    u32 next = b0Link->next;
    u32 endAddr = b0->offset + b0->size;
    while (next != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragLink_t* b1Link = &da->sharedLinks[next];
        DefragMemoryblock_t* b1 = &da->blocks[next];
        u32 b1next = b1Link->next;
        if (b1->offset == endAddr)// b0 and b1 form a continuous memory block
        {
            DefragAllocator_MergeBlocks(da,index,next);
            endAddr = b0->offset + b0->size;
        }
        else // used blocks in between free blocks
        {
            break;
        }
        next = b1next;
    }
}

void DefragAllocator_DefragRoutine(DefragAllocator_t* da)
{
#ifdef DEBUG
    u32 index = da->listUsed.head;
    while (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragLink_t* b0Link = &da->sharedLinks[index];
        DefragMemoryblock_t* b0 = &da->blocks[index];
        u32 next = b0Link->next;
        while (next != DEFRAG_ALLOCATOR_INVALID_INDEX)
        {
            DefragLink_t* b1Link = &da->sharedLinks[next];
            DefragMemoryblock_t* b1 = &da->blocks[next];
            assert(b0->offset != b1->offset);
            next = b1Link->next;
        }
        index = b0Link->next;
    }
#endif
    u32 maxBytesToMove = 16 * 1024;
    u32 tries = 16;
    while (tries)
    {
        if (!DefragAllocator_HasMoreThanOneFreeBlock(da))
        {
            return;
        }
        u32 block = DefragAllocator_MoveLowestFreeBlock(da,&maxBytesToMove);
        if (block != DEFRAG_ALLOCATOR_INVALID_INDEX)
        {
            DefragAllocator_TryMergeFreeBlock(da,block);
        }
        else
        {
            return;
        }
        if (maxBytesToMove == 0)
        {
            return;
        }
        --tries;
    }
}

void DefragCache_Init(DefragCache_t* dc, u32 sizeInBytes, u32 cachePurgeInBytes, u32 maxAllocations)
{
    DefragAllocator_Init(&dc->mem,sizeInBytes,maxAllocations);
    dc->cachePurgeSize = cachePurgeInBytes;
    dc->links = AlignedMalloc(maxAllocations * sizeof(DefragLink_t),4);
    DefragCache_Reset(dc);
}

void DefragCache_Free(DefragCache_t* dc)
{
    AlignedFree(dc->links);
    DefragAllocator_Free(&dc->mem);
}

void DefragCache_Reset(DefragCache_t* dc)
{
    DefragAllocator_Reset(&dc->mem);
    dc->list.head = dc->list.tail = DEFRAG_ALLOCATOR_INVALID_INDEX;
    for (u32 i=0; i<dc->mem.maxAllocations; i++)
    {
        dc->links[i].next = DEFRAG_ALLOCATOR_INVALID_INDEX;
        dc->links[i].prev = DEFRAG_ALLOCATOR_INVALID_INDEX;
    }
}

void* DefragCache_GetPointer(DefragCache_t* dc, u32 handle, u8 type)
{
    void* p = DefragAllocator_GetPointer(&dc->mem,handle,type);
    if (p != NULL)
    {
        u32 index = handle & 0xFFFF;
        DefragLinkedlist_RemoveFrom(dc->links,&dc->list,index);
        DefragLinkedlist_Insert(dc->links,&dc->list,DEFRAG_LIST_AT_HEAD,index);
    }
    return p;
}

void DefragCache_Deallocate(DefragCache_t* dc, u32 handle, u8 type);

u32 DefragCache_Allocate(DefragCache_t* dc, u32 sizeInBytes, u8 type, u32 maxTries)
{
    while (maxTries--)
    {
        u32 handle = DefragAllocator_Allocate(&dc->mem,sizeInBytes,type);
        if (handle)
        {
            u32 index = handle & 0xFFFF;
            DefragLinkedlist_Insert(dc->links,&dc->list,DEFRAG_LIST_AT_HEAD,index);
            return handle;
        }
        else
        {
            // running out of useable memory
            if (dc->mem.freeSize < sizeInBytes)
            {
                // prepare for more allocations
                // cachePurgeSize of extra space
                u32 desiredFreeSize = sizeInBytes + dc->cachePurgeSize;
                while (dc->mem.freeSize < desiredFreeSize)
                {
                    if (dc->list.tail != DEFRAG_ALLOCATOR_INVALID_INDEX)
                    {
                        u32 lastCacheIndex = DefragLinkedlist_Remove(dc->links,&dc->list,DEFRAG_LIST_AT_TAIL);
                        DefragInfo_t* info = &dc->mem.infos[lastCacheIndex];
                        // creates correct handle for DefragAllocator_Deallocate
                        u32 h = lastCacheIndex | (((u32)info->gen) << 16);
                        // todo
                        // add "fast" path with minimal checking for cache clearing?
                        DefragCache_Deallocate(dc,h,info->type);
                    }
                    else
                    {
                        break;
                    }
                }
                printf("defrag cache purge\n");
            }
            else
            {
                // memory is fragmented but has enough space for allocation
            }
            DefragAllocator_DefragRoutine(&dc->mem);
        }
    }
    return 0;
}

void DefragCache_Deallocate(DefragCache_t* dc, u32 handle, u8 type)
{
    u32 index = DefragAllocator_DeallocateAndReturnIndex(&dc->mem,handle,type);
    if (index != DEFRAG_ALLOCATOR_INVALID_INDEX)
    {
        DefragLinkedlist_RemoveFrom(dc->links,&dc->list,index);
    }
}
