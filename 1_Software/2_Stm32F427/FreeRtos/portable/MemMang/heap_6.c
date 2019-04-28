/**
  ******************************************************************************
  * @file    heap_6.c
  * @author  zwx
  * @version V1.8.0
  * @date    2019-01-29
  * @brief   基于heap_4.c 扩展了外部内存管理/分配
  ******************************************************************************
  */

#include "heap_6.h"
#include <stdlib.h>


/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock;   /*<< The next free block in the list. */
    size_t xBlockSize;                      /*<< The size of the free block. */
} BlockLink_t;
/* Private struct  -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE  ( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE       ( ( size_t ) 8 )

#define OS_ENTER_CRITICAL() taskENTER_CRITICAL()
#define OS_EXIT_CRITICAL()  taskEXIT_CRITICAL()

/* Private macro -------------------------------------------------------------*/
/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE
/* Private variables ---------------------------------------------------------*/
//内存池大小
size_t configMyTOTAL_HEAP_SIZE[] =
{
#if (EN_SRAM_IN == 1)
    configTOTAL_HEAP_SIZE1,     //内部内存
#endif
#if (EN_SRAM_EX == 1)
    configTOTAL_HEAP_SIZE2,     //外部内存
#endif
#if (EN_SRAM_CCM == 1)
    configTOTAL_HEAP_SIZE3,     //CCM  此部分仅cpu可以访问
#endif
};

/* Allocate the memory for the heap. */
#if( configAPPLICATION_ALLOCATED_HEAP == 0 )
/* The application writer has already defined the array used for the RTOS
heap - probably so it can be placed in a special segment or address. */
#if (EN_SRAM_IN == 1)
__align(8) static uint8_t ucMemSRAMIn[ configTOTAL_HEAP_SIZE1 ];
#endif
#if (EN_SRAM_EX == 1)
__align(8) static uint8_t ucMemSRAMEx[ configTOTAL_HEAP_SIZE2 ]  __attribute__((at(0X68000000)));
#endif
#if (EN_SRAM_CCM == 1)
__align(8) static uint8_t ucMemSRAMCCM[ configTOTAL_HEAP_SIZE3 ] __attribute__((at(0X10000000)));
#endif
static uint8_t *ucHeap[] =
{
#if (EN_SRAM_IN == 1)
    ucMemSRAMIn,     //内部内存池地址
#endif
#if (EN_SRAM_EX == 1)
    ucMemSRAMEx,     //外部内存池地址
#endif
#if (EN_SRAM_CCM == 1)
    ucMemSRAMCCM,   //CCM 内存池地址(此部分 SRAM 仅仅 CPU 可以访问!!!)
#endif
};
#endif /* configAPPLICATION_ALLOCATED_HEAP */



/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
//让xHeapStructSize的大小字节对齐
static const size_t xHeapStructSize = (sizeof(BlockLink_t) + ((size_t)(portBYTE_ALIGNMENT - 1))) & ~((size_t) portBYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart[COUT_OF_MEM] =
{
#if COUT_OF_MEM > 0
    NULL,
#endif
#if COUT_OF_MEM > 1
    NULL,
#endif
#if COUT_OF_MEM > 2
    NULL,
#endif
};
static BlockLink_t *pxEnd[COUT_OF_MEM] =
{
#if COUT_OF_MEM > 0
    NULL,
#endif
#if COUT_OF_MEM > 1
    NULL,
#endif
#if COUT_OF_MEM > 2
    NULL,
#endif
};

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining[COUT_OF_MEM] =
{
#if COUT_OF_MEM > 0
    0U,
#endif
#if COUT_OF_MEM > 1
    0U,
#endif
#if COUT_OF_MEM > 2
    0U,
#endif
};
static size_t xMinimumEverFreeBytesRemaining[COUT_OF_MEM] =
{
#if COUT_OF_MEM > 0
    0U,
#endif
#if COUT_OF_MEM > 1
    0U,
#endif
#if COUT_OF_MEM > 2
    0U,
#endif
};

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

/* Private functions ---------------------------------------------------------*/
/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvMyInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert,uint8_t eMemType);
/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMalloc() is called.
 */
static void prvMyHeapInit(uint8_t eMemType);

/**
  * @brief
  * @param  None
  * @retval None
  */
void *pvMyPortMalloc(size_t xWantedSize,uint8_t eMemType)
{
    BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;

    if(eMemType >= COUT_OF_MEM)
    {
        return NULL;
    }

    OS_ENTER_CRITICAL();
    {
        /* If this is the first call to malloc then the heap will require
        initialisation to setup the list of free blocks. */
        /*分配时还没有初始化，先初始化*/
        if(pxEnd[eMemType] == NULL)
        {
            prvMyHeapInit(eMemType);
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        /* Check the requested block size is not so large that the top bit is
        set.  The top bit of the block size member of the BlockLink_t structure
        is used to determine who owns the block - the application or the
        kernel, so it must be free. */
        /*用了BlockLink_t中xBlockSize的最高一位来标识某个内存块是否处于空闲状态,
        每一个分配出去的内存块大小就有限制了。例如，我用的是STM32F407，size_t是
        定义为unsigned int类型的，32位，可支持到4G的内存空间。但是最高1位用来指
        示空间状态的话，那就只有31位去标识内存块地址，即只支持到2G的内存空间*/
        if((xWantedSize & xBlockAllocatedBit) == 0)
        {
            /* The wanted size is increased so it can contain a BlockLink_t
            structure in addition to the requested amount of bytes. */
            if(xWantedSize > 0)
            {
                xWantedSize += xHeapStructSize;                 //实际大小=请求大小+1个BlockLink_t的大小

                /* Ensure that blocks are always aligned to the required number
                of bytes. */
                /*保证分配的的大小字节对齐*/
                if((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00)
                {
                    /* Byte alignment required. */
                    xWantedSize += (portBYTE_ALIGNMENT - (xWantedSize & portBYTE_ALIGNMENT_MASK));
                    configASSERT((xWantedSize & portBYTE_ALIGNMENT_MASK) == 0);
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            /*分配大小小于实际可用，可以分配*/
            if((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining[eMemType]))
            {
                /* Traverse the list from the start (lowest address) block until
                one of adequate size is found. */
                /*寻找一块满足分配大小的内存块*/
                pxPreviousBlock = &(xStart[eMemType]);
                pxBlock = xStart[eMemType].pxNextFreeBlock;
                while((pxBlock->xBlockSize < xWantedSize) && (pxBlock->pxNextFreeBlock != NULL))
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }

                /* If the end marker was reached then a block of adequate size
                was not found. */
                /*找到满足大小的内存块了*/
                if(pxBlock != pxEnd[eMemType])
                {
                    /* Return the memory space pointed to - jumping over the
                    BlockLink_t structure at its start. */
                    pvReturn = (void *)(((uint8_t *) pxPreviousBlock->pxNextFreeBlock) + xHeapStructSize);

                    /* This block is being returned for use so must be taken out
                    of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                    /* If the block is larger than required it can be split into
                    two. */
                    /*如果分配出去的空闲块比申请的空间大很多，则将该空闲块进行分割，把剩余的部分
                    重新添加到链表中。*/
                    if((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE)
                    {
                        /* This block is to be split into two.  Create a new
                        block following the number of bytes requested. The void
                        cast is used to prevent byte alignment warnings from the
                        compiler. */
                        pxNewBlockLink = (void *)(((uint8_t *) pxBlock) + xWantedSize);
                        configASSERT((((size_t) pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

                        /* Calculate the sizes of two blocks split from the
                        single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;

                        /* Insert the new block into the list of free blocks. */
                        prvMyInsertBlockIntoFreeList((pxNewBlockLink),eMemType);
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    xFreeBytesRemaining[eMemType] -= pxBlock->xBlockSize;

                    if(xFreeBytesRemaining[eMemType] < xMinimumEverFreeBytesRemaining[eMemType])
                    {
                        xMinimumEverFreeBytesRemaining[eMemType] = xFreeBytesRemaining[eMemType];
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* The block is being returned - it is allocated and owned
                    by the application and has no "next" block. */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;
                    pxBlock->pxNextFreeBlock = NULL;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceMALLOC(pvReturn, xWantedSize);
    }
    OS_EXIT_CRITICAL();

    /*内存分配失败时调用的钩子函数*/
#if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if(pvReturn == NULL)
        {
            extern void vApplicationMallocFailedHook(void);
            vApplicationMallocFailedHook();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
#endif

    configASSERT((((uint32_t) pvReturn) & portBYTE_ALIGNMENT_MASK) == 0);
    return pvReturn;
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void vMyPortFree(void *pv,uint8_t eMemType)
{
    uint8_t *puc = (uint8_t *) pv;
    BlockLink_t *pxLink;

    if(eMemType >= COUT_OF_MEM)
    {
        return;
    }

    if(pv != NULL)
    {
        /* The memory being freed will have an BlockLink_t structure immediately
        before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = (void *) puc;

        /* Check the block is actually allocated. */
        configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
        configASSERT(pxLink->pxNextFreeBlock == NULL);

        if((pxLink->xBlockSize & xBlockAllocatedBit) != 0)
        {
            if(pxLink->pxNextFreeBlock == NULL)
            {
                /* The block is being returned to the heap - it is no longer
                allocated. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;

                OS_ENTER_CRITICAL();
                {
                    /* Add this block to the list of free blocks. */
                    xFreeBytesRemaining[eMemType] += pxLink->xBlockSize;
                    //traceFREE( pv, pxLink->xBlockSize );
                    prvMyInsertBlockIntoFreeList(((BlockLink_t *) pxLink),eMemType);
                }
                OS_EXIT_CRITICAL();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
}

/**
  * @brief
  * @param  None
  * @retval None
  */
size_t xMyPortGetFreeHeapSize(uint8_t eMemType)
{
    if(eMemType >= COUT_OF_MEM)
    {
        return 0;
    }
    else
    {
        return xFreeBytesRemaining[eMemType];
    }
}

/**
  * @brief
  * @param  None
  * @retval None
  */
size_t xMyPortGetMinimumEverFreeHeapSize(uint8_t eMemType)
{
    if(eMemType >= COUT_OF_MEM)
    {
        return 0;
    }
    {
        return xMinimumEverFreeBytesRemaining[eMemType];
    }
}

/**
  * @brief
  * @param  None
  * @retval None
  */
void vMyPortInitialiseBlocks(void)
{
    /* This just exists to keep the linker quiet. */
}
/**
  * @brief
  * @param  None
  * @retval None
  */
static void prvMyHeapInit(uint8_t eMemType)
{
    BlockLink_t *pxFirstFreeBlock;
    uint8_t *pucAlignedHeap;
    size_t uxAddress;
    size_t xTotalHeapSize = configMyTOTAL_HEAP_SIZE[eMemType];

    /* Ensure the heap starts on a correctly aligned boundary. */
    /*保证内存池首地址8字节对齐*/
    uxAddress = (size_t)(ucHeap[eMemType]);

    if((uxAddress & portBYTE_ALIGNMENT_MASK) != 0)
    {
        uxAddress += (portBYTE_ALIGNMENT - 1);
        uxAddress &= ~((size_t) portBYTE_ALIGNMENT_MASK);
        xTotalHeapSize -= uxAddress - (size_t)(ucHeap[eMemType]);
    }

    pucAlignedHeap = (uint8_t *) uxAddress;

    /* xStart is used to hold a pointer to the first item in the list of free
    blocks.  The void cast is used to prevent compiler warnings. */
    xStart[eMemType].pxNextFreeBlock = (void *) pucAlignedHeap;               //指向第一个可用内存块
    xStart[eMemType].xBlockSize = (size_t) 0;

    /* pxEnd is used to mark the end of the list of free blocks and is inserted
    at the end of the heap space. */
    /*指向了内存堆的最后一个BlockLink_t大小的位置上。也就是说，内存堆最后的空间是存储着一个BlockLink_t，用来指示空闲块链表的终结*/
    uxAddress = ((size_t) pucAlignedHeap) + xTotalHeapSize;
    uxAddress -= xHeapStructSize;
    uxAddress &= ~((size_t) portBYTE_ALIGNMENT_MASK);
    pxEnd[eMemType] = (void *) uxAddress;
    pxEnd[eMemType]->xBlockSize = 0;
    pxEnd[eMemType]->pxNextFreeBlock = NULL;

    /* To start with there is a single free block that is sized to take up the
    entire heap space, minus the space taken by pxEnd. */
    /*初始只有一个可用内存块，大小为所有可用内存池大小*/
    pxFirstFreeBlock = (void *) pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize = uxAddress - (size_t) pxFirstFreeBlock;
    pxFirstFreeBlock->pxNextFreeBlock = pxEnd[eMemType];

    /* Only one block exists - and it covers the entire usable heap space. */
    xMinimumEverFreeBytesRemaining[eMemType] = pxFirstFreeBlock->xBlockSize;
    xFreeBytesRemaining[eMemType] = pxFirstFreeBlock->xBlockSize;

    /* Work out the position of the top bit in a size_t variable. */
    /*用了BlockLink_t中xBlockSize的最高一位来标识某个内存块是否处于空闲状态*/
    xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}


/**
  * @brief
  * @param  None
  * @retval None
  */
static void prvMyInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert,uint8_t eMemType)
{
    BlockLink_t *pxIterator;
    uint8_t *puc;

    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for(pxIterator = &(xStart[eMemType]); pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock)
    {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    /*先试着将pxIterator和pxBlockToInsert进行合并，可以合并的标准为pxIterator的
    首地址加上pxIterator的块大小之后等于pxBlockToInsert的首地址。相等就说明两个
    块是相邻的.如果不能合并，就什么事都不做。*/
    puc = (uint8_t *) pxIterator;
    if((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert)
    {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    /*再试着将pxBlockToInsert和pxIterator指向的下一个空闲块进行合并。可合并的标
    准和刚刚说的一样，只是这次用pxBlockToInsert的首地址加上pxBlockToInsert的块大
    小与pxIterator指向的下一个块地址比较。能合并是最好的，不能合并，则要修改
    pxBlockToInsert的Next指针，指向pxIterator的下一个空闲块*/
    puc = (uint8_t *) pxBlockToInsert;
    if((puc + pxBlockToInsert->xBlockSize) == (uint8_t *) pxIterator->pxNextFreeBlock)
    {
        if(pxIterator->pxNextFreeBlock != pxEnd[eMemType])
        {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = pxEnd[eMemType];
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    /*最后，要是pxBlockToInsert没有和pxIterator合并，则还要修改pxIterator的Next
    指针，这样整条链表才完整无误*/
    if(pxIterator != pxBlockToInsert)
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}


/****************** 北京龙鼎源科技 http://www.bdrl.com.cn/ ********************/

