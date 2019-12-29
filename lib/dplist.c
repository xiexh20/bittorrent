#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1  // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...)                                                                    \
    do                                                                                       \
    {                                                                                        \
        fprintf(stderr, "\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                        \
        fflush(stderr);                                                                      \
    } while (0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif

#define DPLIST_ERR_HANDLER(condition, err_code)   \
    do                                            \
    {                                             \
        if ((condition))                          \
            DEBUG_PRINTF(#condition " failed\n"); \
        assert(!(condition));                     \
    } while (0)

/*
 * The real definition of struct list / struct node
 */

struct dplist_node
{
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist
{
    dplist_node_t *head;
    void *(*element_copy)(void *src_element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
    void (*element_print)(void *element);
};

dplist_t *dpl_create( // callback functions
    void *(*element_copy)(void *src_element),
    void (*element_free)(void **element),
    int (*element_compare)(void *x, void *y))
    // void (*element_print)(void * element))
{
    // printf("Test makefile build static library.\n");
    // printf("Test makefile build static library again.\n");
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    // list->element_print = element_print;
    return list;
}

// Every list node of the list needs to be deleted (free memory)
// If free_element == true : call element_free() on the element of the list node to remove
// If free_element == false : don't call element_free() on the element of the list node to remove
// The list itself also needs to be deleted (free all memory)
// '*list' must be set to NULL.
// Extra error handling: use assert() to check if '*list' is not NULL at the start of the function. 
void dpl_free(dplist_t **list, bool free_element)
{
    // add your code here
    assert(*list != NULL);
    while(dpl_size(*list)>0){
        dpl_remove_at_index(*list, 0, free_element);
    }
    
    free(*list);    // also free the pointer to the list
    *list = NULL;   // clear the list
}

// Inserts a new list node containing an 'element' in the list at position 'index' and returns a pointer to the new list.
// If insert_copy == true : use element_copy() to make a copy of 'element' and use the copy in the new list node
// If insert_copy == false : insert 'element' in the new list node without taking a copy of 'element' with element_copy() 
// Remark: the first list node has index 0.
// If 'index' is 0 or negative, the list node is inserted at the start of 'list'. 
// If 'index' is bigger than the number of elements in the list, the list node is inserted at the end of thelist.
dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy)
{
    dplist_node_t *ref_at_index, *list_node;
    // create a list node
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    list_node = malloc(sizeof(dplist_node_t));      
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    if(insert_copy){
        // use deep copy
        // list_node = malloc(sizeof(dplist_node_t));      // only allocate new memory when deep copy is used
        list_node->element = list->element_copy(element);
    }
    else{
        // do not use deep copy, onlt assign pointer
        // list->element_free(&list_node->element);
        // free(list_node->element);
        list_node->element = element;
    }

    if (list->head == NULL)
    { // covers case 1: an empty list, insert directly to the end of the list
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    }
    else if (index <= 0)
    { // covers case 2, insert to the head of the list
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node; // head also has a value
        // pointer drawing breakpoint
    }
    else
    {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL); // if index too large, insert to the end
        // if(ref_at_index==NULL){
        //     // insert into the end of the list
        //     ref_at_index = malloc(sizeof(dplist_node_t));

        // }
        
        if (index < dpl_size(list))
        { // covers case 4: in the middle of the list
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        }
        else
        { // covers case 3, at the end of the list
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            
        }
    }
    return list;
}

// Removes the list node at index 'index' from the list. 
// If free_element == true : call element_free() on the element of the list node to remove
// If free_element == false : don't call element_free() on the element of the list node to remove
// The list node itself should always be freed
// If 'index' is 0 or negative, the first list node is removed. 
// If 'index' is bigger than the number of elements in the list, the last list node is removed.
// If the list is empty, return the unmodifed list 
dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element)
{
    if(list == NULL || list->head == NULL){
        // empty list, return original list
        return list;
    }

    dplist_node_t * deleted = dpl_get_reference_at_index(list, index);  // a pointer to the node to be deleted
    if(deleted == NULL){
        return list;    // list is empty
    }

    // printf("Element to be deleted:");
    // list->element_print(deleted->element);      // reference is correct
    // change reference 
    dplist_node_t* prev = deleted->prev;
    dplist_node_t* next = deleted->next;

    if(prev==NULL){
        // this is a node in the head
        if(next!=NULL){
            next->prev = NULL;
            list->head = next;
        }
        else{
            // the only node in the list is deleted, clear head pointer
            list->head = NULL;
        }
    }
    else{
        // a node not in the head of the list
        prev->next = next;
        if(next!=NULL){
            // at the middle of the list
            next->prev = prev;
        }
        else{
            
        }
    }
    
    if(free_element){
        list->element_free((void*)&(deleted->element));
        free(deleted);      // also free memory for the node
        deleted = NULL; // make sure it is empty
    }
    return list;
    
}

int dpl_size(dplist_t *list)
{
    int count = 0;
    if(list==NULL){
        return -1;   // error
    }
    dplist_node_t* node = list->head;
    while (node!=NULL)
    {
        node = node->next;
        count++;
    }
    return count;
    
}

// Returns the list element contained in the list node with index 'index' in the list.
// Remark: return is not returning a copy of the element with index 'index', i.e. 'element_copy()' is not used. 
// If 'index' is 0 or negative, the element of the first list node is returned. 
// If 'index' is bigger than the number of elements in the list, the element of the last list node is returned.
// If the list is empty, (void *)0 is returned.
void *dpl_get_element_at_index(dplist_t *list, int index)
{
    if(list == NULL || list->head == NULL){
        return (void*)0;    // NULL list
    }
    if(index<=0){
        // index is 0 or negative, return the head of the list
        return list->head->element;
    }
    int count = 0;
    dplist_node_t *temp = list->head;
    while (temp->next != NULL)
    {
        if (count == index)
        {
            return temp->element;   // index matched
        }
        temp = temp->next;
        count++;
    }
    return temp->element;    // return the tail of the list
}

// Returns an index to the first list node in the list containing 'element'.
// Use 'element_compare()' to search 'element' in the list
// A match is found when 'element_compare()' returns 0
// If 'element' is not found in the list, -1 is returned.
int dpl_get_index_of_element(dplist_t *list, void *element)
{
    int count = 0;
    dplist_node_t *temp = list->head;
    while (temp != NULL)
    {
        if (list->element_compare(element, temp->element) == 0)
        {
            return count;   // element found
        }
        temp = temp->next;
        count++;
    }
    return -1; // element not found, return -1
    
}

// Returns a reference to the list node with index 'index' in the list. 
// If 'index' is 0 or negative, a reference to the first list node is returned. 
// If 'index' is bigger than the number of list nodes in the list, a reference to the last list node is returned.xxx NULL is returned
// If the list is empty, NULL is returned.
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index)
{
    int count;
    dplist_node_t *dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    if (list->head == NULL)
        return NULL;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++)
    {
        if (count >= index)
            return dummy; // if count >= index
    }
    return dummy; // if not find, return NULL

}

// Returns the element contained in the list node with reference 'reference' in the list. 
// If the list is empty, NULL is returned. 
// If 'reference' is NULL, the element of the last element is returned.
// If 'reference' is not an existing reference in the list, NULL is returned.
void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference)
{

    if(list == NULL || list->head == NULL){
        // empty list
        return NULL;
    }
    dplist_node_t* temp = list->head;
    while (temp->next != NULL)
    {
        if(temp == reference){
            return temp->element;
        }
        temp = temp->next;
    }
    if(reference == NULL){
        return temp->element;    // reference is NULL, return the tail of the list
    }
    else if(temp==reference){
        return temp->element;   // check tail
    }
    else{
        return NULL; // reference not found, return NULL
    }
    
    
}


// Returns the index of the list node in the list with reference 'reference'. 
// If the list is empty, -1 is returned. 
// If 'reference' is NULL, the index of the last element is returned.
// If 'reference' is not an existing reference in the list, -1 is returned.
int dpl_get_index_of_reference( dplist_t * list, dplist_node_t * reference )
{
    if(list == NULL || list->head == NULL){
        // empty list
        return -1;
    }
    int index = 0;
    dplist_node_t* temp = list->head;
    while (temp->next != NULL)
    {
        if(temp == reference){
            return index;
        }
        temp = temp->next;
        index ++;
    }
    if(reference==NULL){
        return index;       // reference is null, return last element index
    }
    else{
        // check the last reference
        if(temp==reference){
            return index;
        }
        else{
            return -1;      // non-existing reference
        }
        
    }
}

// Returns a reference to the next list node of the list node with reference 'reference' in the list. 
// If the list is empty, NULL is returned
// If 'reference' is NULL, NULL is returned.
// If 'reference' is not an existing reference in the list, NULL is returned.
dplist_node_t * dpl_get_next_reference( dplist_t * list, dplist_node_t * reference )
{
    if(list == NULL || list->head == NULL || reference == NULL){
        // empty list
        return NULL;
    }
    int index = dpl_get_index_of_reference(list, reference);
    if(index>=0){
        return reference->next;
    }
    return NULL;
}

// Returns a reference to the previous list node of the list node with reference 'reference' in 'list'. 
// If the list is empty, NULL is returned.
// If 'reference' is NULL, a reference to the last list node in the list is returned.
// If 'reference' is not an existing reference in the list, NULL is returned.
dplist_node_t * dpl_get_previous_reference( dplist_t * list, dplist_node_t * reference )
{
    if(list == NULL || list->head == NULL || reference == NULL){
        // empty list
        return NULL;
    }
    
    int index = dpl_get_index_of_reference(list, reference);
    if(index>=0){
        return reference->prev;
    }
    return NULL;
}

// Removes the list node with reference 'reference' in the list. 
// If free_element == true : call element_free() on the element of the list node to remove
// If free_element == false : don't call element_free() on the element of the list node to remove
// The list node itself should always be freed
// If 'reference' is NULL, the last list node is removed.
// If 'reference' is not an existing reference in the list, 'list' is returned.
// If the list is empty, return the unmodifed list
dplist_t * dpl_remove_at_reference( dplist_t * list, dplist_node_t * reference, bool free_element )
{
    if(list == NULL || list->head == NULL){
        // empty list
        return list;
    }

    int index = dpl_get_index_of_reference(list, reference);
    if(index<0){
        return list;
    }
    else{
        return dpl_remove_at_index(list, index, free_element);
    }
}


// Returns a reference to the first list node in the list containing 'element'. 
// If the list is empty, NULL is returned. 
// If 'element' is not found in the list, NULL is returned.
dplist_node_t * dpl_get_reference_of_element( dplist_t * list, void * element )
{
    if(list == NULL || list->head == NULL){
        // empty list
        return NULL;
    }
    int index = dpl_get_index_of_element(list, element);
    if(index<0){
        return NULL;    // element not found
    }
    else{
        return dpl_get_reference_at_index(list, index);
    }

}

// Returns a reference to the first list node of the list. 
// If the list is empty, NULL is returned.
dplist_node_t * dpl_get_first_reference( dplist_t * list )
{
    if(list == NULL || list->head == NULL){
        // empty list
        return NULL;
    }
    return list->head;
}

// Returns a reference to the last list node of the list. 
// If the list is empty, NULL is returned.
dplist_node_t * dpl_get_last_reference( dplist_t * list )
{
    if(list == NULL || list->head == NULL){
        // empty list
        return NULL;
    }
    return dpl_get_reference_at_index(list, dpl_size(list));    // return the last node refernce
}
