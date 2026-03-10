#include "include/utils.h"
#include <assert.h>
#include <stdlib.h>

#define  numLeonardoNumbers  46
#define  MAX_LINE_LENGTH     1000
#define  MAX_ARRAY_SIZE      10000000

/* Precomputed values of the first 46 Leonardo numbers */
const size_t leonardoList[numLeonardoNumbers] = 
{   1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 
    1219, 1973, 3193, 5167, 8361, 13529, 21891, 35421, 57313, 92735,
    150049, 242785, 392835, 635621, 1028457, 1664079, 2692537, 
    4356617, 7049155, 11405773, 18454929, 29860703, 48315633, 78176337,
    126491971, 204668309, 331160281, 535828591, 866988873, 1402817465,
    2269806339, 3672623805
};

/* Structure containing the "modified bit vector" description of the
 * heap -- comprised of a bit vector implemented as a size_t word and the 
 * Leonardo index ("order") of the smallest heap present.
 */
typedef struct {
    size_t bitVector;
    size_t lastTreeOrder;
} heapStatus;

/* Structure for storing the results of a swapToProperHeap function call,
 * holding the number of swaps performed, the new index of the original node,
 * and the order of the tree that the original node is now atop */
typedef struct {
    size_t numSwaps;
    size_t newRootInd;
    size_t newRootOrder;
} swapInfo;



/**
 * Function: bool isLastTree(size_t rootIndex, size_t rootOrder)
 * ---------------------------------------------------------
 *  Given the index of a node in the array, and the order of the node,
 *  returns true if the tree rooted at the specified node is the last
 *  tree in the array.
 */
static bool isLastTree(size_t rootIndex, size_t rootOrder)
{   return (rootIndex < leonardoList[rootOrder]);
}


/**
 * Function: size_t rightChild(size_t rootIndex)
 * ---------------------------------------------------------
 *  Given the index of a node in the array, returns the array index of 
 *  that node's right child.
 */
static size_t rightChild(size_t rootIndex)
{   /* The right child of any node is always adjacent to that node */
    return (rootIndex - 1);
}


/**
 * Function: size_t leftChild(size_t rootIndex, size_t rootOrder)
 * ---------------------------------------------------------
 *  Given the index of a node in the array and the size of the
 *  tree rooted at that index, returns the array index of that node's
 *  left child.
 */
static size_t leftChild (size_t rootIndex, size_t rootOrder)
{   /* The left child of any node is adjacent to the last leaf in the 
     * right child's tree. The right child is always of two orders
     * less than its parent. */
    return (rightChild(rootIndex) - leonardoList[rootOrder - 2]);
}


/**
 * Function: size_t nextTree(size_t rootIndex, size_t rootSize)
 * ---------------------------------------------------------
 *  Given the index of a node in the array and the size of the
 *  tree rooted at that index, returns the array index of that node's
 *  nearest neighboring tree.
 */
static size_t nextTree(size_t rootIndex, size_t rootOrder)
{   /* The next tree is always adjacent to the last leaf in the previous
     * tree, separated from the current tree's root by the size of the
     * current tree */
    return (rootIndex - leonardoList[rootOrder]);
}

/*
 * Function: bool findNextNode(size_t *bitVector, size_t *nodeOrder)
 * ---------------------------------------------------------
 * Given pointers to a size_t bit vector (containing the current node, so
 * must be >= 1) and the current node's order, finds the next node in the 
 * list and stores it in currentOrder. This function MODIFIES bitVector, 
 * aligning the next node (of order nodeOrder) to the right end of the 
 * bit vector. Returns true if there a node of greater order was found 
 * in the heap (true if nodeOrder was changed).
 */
static bool findNextNode(size_t *bitVector, size_t *nodeOrder)
{   if (*bitVector == 1) /*|| (*nodeOrder == 0)) */
    {   /* If we are at the final node, there is not
         * another node to be found */
        return false;
    }

    while (true)
    {   (*bitVector) = ((*bitVector) >> 1);
        (*nodeOrder)++;
        if (((*bitVector) & 1) == 1)
        {   return true;
        }
        else if ((*bitVector) == 0)
        {   /* If the bit vector is 0, and we haven't
             * found another node, then we were at the last node */
            return false;
        }
    }
}

static void siftRootDown(arr_t arr, size_t rootInd, size_t rootOrder, _compar cmp)
{   size_t rChildInd, lChildInd;
    if (rootOrder == 1 || rootOrder == 0)
    {   /* If our heap is of size 1, no shifting to be done */
        return;
    }

    /* Right child of a node is always adjacent to it */
    rChildInd = rootInd - 1;
    /* Right child of a Leonardo heap is always 2 "Leonardo indices"
     * smaller than the root node, and left child is adjacent to the
     * right child heap */
    lChildInd = rChildInd - leonardoList[rootOrder - 2];

    /* TODO Sift down such that larger value is always the 
     *      right child? Will this actually speed things up? */
    /* TODO Declare a ` swapChild ' and ` swapOrder ' so there's 
     *      only one call to swapValues and siftRootDown? */
    if (
        cmp(_at(arr, rChildInd), _at(arr, lChildInd)) &&
        cmp(_at(arr, rootInd), _at(arr, lChildInd))
    )
    {  /* Left child is larger than the right child and the root 
        * node, so switch left child and root. */
        swap_bytes(_at(arr, rootInd), _at(arr, lChildInd), arr.byte_size);
        /* Continue sifting down the original root value
         * while observing that the left child has order
         * rootOrder - 1 in a Leonardo heap */
        siftRootDown(arr, lChildInd, (rootOrder - 1), cmp);
    }
    else if (cmp(_at(arr, rootInd), _at(arr, rChildInd)))
    {   /* Right child is larger than the left child, and larger than the
         * root node, so switch right child and root. */
        swap_bytes(_at(arr, rootInd), _at(arr, rChildInd), arr.byte_size);

        /* Continue sifting down, note  right child has order
         * rootOrder - 2 in a Leonardo heap */
        siftRootDown(arr, rChildInd, (rootOrder - 2), cmp);
    }

    /* If neither child is larger than the root node, there's no need to
     * switch anything, so return. */
    return;
}

static swapInfo* swapToProperHeap(arr_t arr, size_t rootIndR, 
                           const heapStatus *hStat, _compar cmp)
{   size_t rootOrderR     = hStat->lastTreeOrder;
    size_t rootSizeR      = leonardoList[rootOrderR];
    /* TODO */
    size_t i;
    size_t rootRChildIndR = rightChild(rootIndR);
    size_t rootRChildIndL = leftChild(rootIndR, rootOrderR);
    size_t rootIndL       = nextTree(rootIndR, rootOrderR);

    size_t rootOrderL = rootOrderR;
    size_t rootSizeL;
    size_t *rootOrderLPtr = &rootOrderL;

    size_t numSwaps    = 0;
    size_t bitVectorCpy;
    size_t *bitVectorCpyPtr = &bitVectorCpy;

#ifndef NDEBUG
    size_t debugVal1, debugVal2;
    char  *debugStr1, *debugStr2;
    size_t origRootInd = rootIndR;
    size_t origLeftInd = rootIndL;
    size_t origRootChildIndR = rootRChildIndR;
    size_t origRootChildIndL = rootRChildIndL;
    bool wasRootSize1 = (rootOrderR < 2); /* was root a singleton? */
    bool bool1, bool2, bool3;
#endif

    swapInfo *returnValPtr; 


    /* Shift bits in the bit vector copy until the first nonzero bit
     * is at the least significant position */
    bitVectorCpy = ((hStat->bitVector) >> rootOrderR);

    findNextNode(bitVectorCpyPtr, rootOrderLPtr);

    /* If we've reached the end of the bitVector, or are at the last
     * node, then there are no more nodes to swap, so return */
    if (bitVectorCpy == 0 || rootIndR == 0 || (isLastTree(rootIndR, rootOrderR)))
    {   /* TODO printf("    LAST NODE, exiting swap!\n\n"); */
        returnValPtr = malloc(sizeof(*returnValPtr));
        returnValPtr->numSwaps     = numSwaps;
        returnValPtr->newRootInd   = rootIndR;
        returnValPtr->newRootOrder = rootOrderR;
        return returnValPtr;
    }

    rootSizeL = leonardoList[rootOrderL];
    
    while (true)
    {   if ((rootOrderR > 1) && (rootIndR >= leonardoList[rootOrderR]))
        {   /* TODO might be able to remove the condition on rootIndR 
             *      using the checks on the bitvector at the end of the
             *      loop 
             *      combine above conditional into break condition? */
            /* If the "right" root node has children nodes and isn't the
             * last tree in the heap */
            rootRChildIndR = rightChild(rootIndR);
            rootRChildIndL = leftChild(rootIndR, rootOrderR);
            rootIndL       = nextTree(rootIndR, rootOrderR);
            
            if(
                (cmp(_at(arr,rootIndR), _at(arr, rootIndL)))  &&
                (cmp(_at(arr, rootRChildIndR), _at(arr, rootIndL))) &&
                (cmp(_at(arr, rootRChildIndL), _at(arr, rootIndL)))
            )
            { 
                /* If the next largest tree has a root node that is larger
                 * than the current heap's root node, and larger than the
                 * children of the current root node, then we must swap the
                 * two root nodes. */
                swap_bytes(_at(arr, rootIndL), _at(arr, rootIndR), arr.byte_size);
                numSwaps++;
            }
            else
            {   /* If the next tree's root is not larger than the current 
                 * root and its children, then we are done swapping */
                break;
            }
        }

        else if (rootOrderR == 1 || rootOrderR == 0) 
        {   /* Root is a singleton node */ 
            rootIndL       = nextTree(rootIndR, rootOrderR);
            if(cmp(_at(arr, rootIndR), _at(arr, rootIndL)))
            {   
                swap_bytes(_at(arr, rootIndL), _at(arr, rootIndR), arr.byte_size);
                numSwaps++;
            }
            else
            {   /* If the next tree's root is not larger than the current 
                 * root and its children, then we are done swapping */
                break;
            }
        }

        else
        {   /* Current tree has no neighboring tree */
            /* Since we're using unsigned values, if there is no next tree its
             * index will be -1, corresponding to the maximum value that 
             * type size_t can hold. */
            assert(nextTree(rootIndR, rootOrderR) > rootIndR);
            break;
        }
        
        /* Here we have just completed a swap operation, so need to find 
         * the next left-adjacent tree, and update our variables to reflect
         * that the "right" tree under consideration is now the former left 
         * tree. */
        rootIndR   = rootIndL; 
        rootOrderR = rootOrderL;

        if (!findNextNode(bitVectorCpyPtr, rootOrderLPtr))
        {   /* If there is not another node to be found */
            break;
        }

        /* Otherwise we have updated our bitVectorCopy and the the order of the
         * neighboring node, so continue to next loop iteration */
    }

    returnValPtr = malloc(sizeof(*returnValPtr));
    returnValPtr->numSwaps     = numSwaps;
    returnValPtr->newRootInd   = rootIndR;
    returnValPtr->newRootOrder = rootOrderR;

    return returnValPtr;
}

static void addToLeonardoHeap(size_t index, arr_t arr, heapStatus *hStat, _compar cmp)
{   
    size_t origLastTree = hStat->lastTreeOrder;
    swapInfo *swapSummary;

    /* When adding to the Leonardo heap, there are 3 cases: that the last
     * two bits in the bit vector are both 1 (adjacent Leonardo heap sizes),
     * that the last tree index is 1 (and therefore we need to add a tree of
     * index 0), or that the second-to-last bit (Leonardo index) is zero,
     * and we therefore need to add a tree of order (index) 1. 
     */

    /* Trivial case: heap is empty, so add Leonardo heap of order 1 */
    if ((hStat->bitVector == 0 && origLastTree == 0))
    {   hStat->bitVector     = 2;
        hStat->lastTreeOrder = 1;
        return; /* No need to do a shiftRootDown operation here */
    }

    /* Case 1: Last 2 bits are ` 11 '
     * Here we need to make sure that the bit adjacent (higher) to
     * the last nonzero bit in our array is nonzero, so we can shift our
     * bit vector until the last nonzero bit is in the least significant
     * place (so the final 2 bits should be ` 11 '), and then take 3, 
     * binary 11, and use a simple bitwise AND to tell us if those two 
     * bits are also nonzero in the original bit vector. Note that bitwise 
     * shifts do NOT change the shifted value without explicit assignment.
     */
    else if ((((hStat->bitVector) >> origLastTree) & 3) == 3) 
    {   /* Now we know that the three "last bits" in our bit vector were 011
         * (max two adjacent Leonardo indices), so we now must combine those
         * two into one larger heap, so the last 3 bits will be 100, and 
         * lastTreeOrder += 2 . To change the last 3 bits, we can take 
         * 7, binary 111, and shift it until the bits align with the "last
         * three bits" in our original vector, then perform binary XOR 
         * (111 ^ 011 = 100). 
         */
        hStat->bitVector = ((7 << origLastTree) ^ (hStat->bitVector));
        hStat->lastTreeOrder += 2;
        assert(((hStat->bitVector >> (hStat->lastTreeOrder - 2)) ^ 7) ==
               ((hStat->bitVector >> (hStat->lastTreeOrder - 2)) - 1));
    }

    /* Case 2: ` lastTreeOrder = 1 '
     * The last nonzero bit in the bit vector is of order 1, so we
     * simply add a tree of order 0, updating the bit array and 
     * decrementing lastTreeOrder 1 (to 0).
     */
    else if (origLastTree == 1)
    {   assert(((hStat->bitVector) ^ 1) == (hStat->bitVector + 1));
        assert(((hStat->bitVector) & 1) == 0);

        /* Here we know that the last two bits are ` 10 ', so we can
         * use binary OR with 1 to update the bit vector. */
        hStat->bitVector = (hStat->bitVector | 1);
        hStat->lastTreeOrder--;
    }
    

    /* Case 3: Last 2 bits are 01, lastTreeOrder != 1
     * Here we simply add a tree of order 1.
     */
    else
    {   assert((((hStat->bitVector) >> origLastTree) & 1) == 1);
        assert((((hStat->bitVector) >> origLastTree) & 2) == 0);

        /* We know that the last two bits are ` 01 ', so we can
         * use binary OR with 2, binary ` 10 ', to update the
         * second-to-last bit. */
        hStat->bitVector = ((hStat->bitVector) | 2);
        hStat->lastTreeOrder = 1;
    }

    /* Now we have "added" the next adjacent index to the array, but
     * we don't know if the heap conditions are still satisfied, so
     * rearrange until they are.
     */
    swapSummary = swapToProperHeap(arr, index, hStat, cmp);

    siftRootDown(arr, swapSummary->newRootInd, swapSummary->newRootOrder, cmp);
    
    free(swapSummary);
    swapSummary = 0;
}


static void removeFromLeonardoHeap(size_t index, arr_t arr, heapStatus *hStat, _compar cmp)
{   
    size_t origLastTree = hStat->lastTreeOrder;
    size_t bitVectorCopy, swapIndex1, swapIndex2, swapOrder1, swapOrder2;
    swapInfo *swapSummary;
    /* When removing from the Leonardo heap, there are 3 cases: that the last
     * tree in the heap is at least order 2, that tree being deleted is of
     * order 0 (and we therefore have a tree of order 1 behind it), or that
     * the root being deleted is from a tree of order 1 (and we must do some
     * bitwise operations to find the next largest tree).
     */
    
    /* Case 1: Last tree in the heap is at least of order 2
     * Here we simply remove the root node, exposing the two children,
     * and thus must invert the last three bits of the bit vector
     * (from ` 100 ' to ` 011 ') and reduce the last tree order by 2.
     * To swap the bits, we can take 7 (binary ` 111 ') and shift until
     * the most significant bit aligns with the last nonzero bit in the
     * bit vector: this amounts to shifting by origLastTree - 2 bits.
     * This is equivalent to subtracing (1 << (origLastTree - 2)) from 
     * the bitvector.
     */
    if (origLastTree >= 2)
    {   /* Assert that the bit vector structure matches our expectations
         * from from the origLastTree value */
        assert((hStat->bitVector | 2) != (hStat->bitVector));
        assert((hStat->bitVector | 1) != (hStat->bitVector));

        hStat->bitVector = ((hStat->bitVector) ^ (7 << (origLastTree - 2)));
        hStat->lastTreeOrder -= 2; /* TODO can take out this line, if
                                      ` swapOrder ' redefined below */

        /* Assert that the last two bits are indeed ` 11 ' */
        assert(((hStat->bitVector >> hStat->lastTreeOrder) & 3) == 3);

        /* Here we will store the indices and orders of the two children
         * trees for usage later in swapToProperHeap and siftRootDown, 
         * swapIndex1 and swapOrder1 store the information about the 
         * final tree, while the other two variables store information 
         * about the penultimate tree. 
         */

        /* FIXME I originally wanted to swap the penultimate tree at
         *       swapIndex2 before swapIndex1, but somehow they got messed
         *       up. Now if I try to fix this, the final above assertion fails,
         *       though the bitvector seems to be OK. Check for orders and such.
         */
        swapIndex1 = leftChild(index, origLastTree);
        swapIndex2 = rightChild(index);

        /* Last tree order has been decremented above to match the order 
         * of the tree rooted at swapIndex2 */
        swapOrder1 = hStat->lastTreeOrder + 1;
        swapOrder2 = hStat->lastTreeOrder;
    }
    
    /* Case 2: Tree being deleted is of order 0
     * Here we must also have an adjacent tree of order 1, so we may simply 
     * remove the order 0 tree, change the last bit of the bit vector, and
     * update the last tree order. In this case there's no need to rearrange
     * the heap (no swapping nor sifting).
     */
    else if (origLastTree == 0)
    {   /* Assert that the last two bits are ` 11 ' as expected */
        assert((hStat->bitVector & 3) == 3);

        hStat->bitVector = ((hStat->bitVector) ^ 1);
        /* Assert that the last bit is indeed 0 */
        assert((hStat->bitVector & 1) == 0);

        hStat->lastTreeOrder++;
        return;
    }

    /* Case 3: Tree being deleted is of order 1
     * Here we will not have any children to "split" the tree into, so
     * we must find the next largest tree after we swap the second bit
     * of the bit vector (XOR with 2 = ` 10 '). Finding the next tree is 
     * simply a matter of bit shifts while inrementing the last tree index. 
     * Note that we also do not have to do any swapping nor sifting here,
     * since there are no "new" heaps introduced.
     */
    else
    {   /* Assert that that the last tree order and bitvector are as we
         * expect */
        assert(hStat->lastTreeOrder == 1);
        assert((hStat->bitVector ^ 2) == (hStat->bitVector - 2));

        hStat->bitVector = ((hStat->bitVector) ^ 2);

        /* Prior to the above switch, the last 2 bits should have been 
         * ` 10 ' since the last tree is of order 1; so they should now
         * both be 0. */
        assert(((hStat->bitVector & 3) == 0));

        /* Now search for the next smallest tree: make a copy of the
         * bit vector (shifted to account for the value that last tree 
         * order currently has), and shit this copy until we have a nonzero final
         * bit, incrementing our last tree order with every shift.
         */
        bitVectorCopy = (hStat->bitVector >> hStat->lastTreeOrder);
        while (bitVectorCopy != 0)
        {   (hStat->lastTreeOrder)++;
            if (((bitVectorCopy = (bitVectorCopy >> 1)) & 1) == 1)
            {
                break;
            }
        }
        return;
    }

    /* If here, then we have 2 "new" trees in the heap, the children of the
     * node that was removed, and we must make sure their roots are atop
     * the proper heaps,then sift the nodes down their new trees. For 
     * the first swap, we ignore the last tree and convert the truncated
     * heap to proper form, then consider the last tree in the second
     * swap and sift operations. */
    hStat->lastTreeOrder = swapOrder1;

    swapSummary = swapToProperHeap(arr, swapIndex1, hStat, cmp);

    if (swapSummary->numSwaps)
    {   
        siftRootDown(arr, swapSummary->newRootInd, swapSummary->newRootOrder, cmp);
    }

    hStat->lastTreeOrder = swapOrder2;
    swapSummary = swapToProperHeap(arr, swapIndex2, hStat, cmp);

    if (swapSummary->numSwaps)
    {   siftRootDown(arr, swapSummary->newRootInd, swapSummary->newRootOrder, cmp);
    }
    
    free(swapSummary);
    swapSummary = 0;
   

}

void smooth_sort(arr_t arr, _compar cmp) {
    heapStatus hStat = {0};
    heapStatus *hStatPtr = &hStat;
    
    for(int i = 0; i < arr.n; i++)
        addToLeonardoHeap(i, arr, hStatPtr, cmp);
    
    for (int i = arr.n - 1; i >= 1; i--) 
        removeFromLeonardoHeap(i, arr, hStatPtr, cmp);
}