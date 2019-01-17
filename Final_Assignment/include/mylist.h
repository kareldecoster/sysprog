#ifndef mylist_H_
#define mylist_H_

extern int mylist_errno;

/*
 * definition of error codes
 * */
#define mylist_NO_ERROR 0
#define mylist_MEMORY_ERROR 1 // error due to mem alloc failure
#define mylist_EMPTY_ERROR 2  //error due to an operation that can't be executed on an empty mylist
#define mylist_INVALID_ERROR 3 //error due to a mylist operation applied on a NULL mylist 
#define mylist_EXTRA

typedef void *element_ptr_t;


typedef struct mylist mylist_t; // mylist_t is a struct containing at least a head pointer to the start of the mylist; 
typedef mylist_t *mylist_ptr_t;

typedef struct mylist_node mylist_node_t;
typedef mylist_node_t *mylist_node_ptr_t;


mylist_ptr_t mylist_create 	( // callback functions
			  void (*element_copy)(element_ptr_t *dest_element, element_ptr_t src_element),
			  void (*element_free)(element_ptr_t *element),
			  int (*element_compare)(element_ptr_t x, element_ptr_t y),
			  void (*element_print)(element_ptr_t element)
			);
// Returns a pointer to a newly-allocated mylist.
// Returns NULL if memory allocation failed and mylist_errno is set to mylist_MEMORY_ERROR 

void mylist_free( mylist_ptr_t* mylist );
// Every mylist node and node element of the mylist needs to be deleted (free memory)
// The mylist itself also needs to be deleted (free all memory) and set to NULL

int mylist_size( mylist_ptr_t mylist );
// Returns the number of elements in 'mylist'.

mylist_ptr_t mylist_insert_at_index( mylist_ptr_t mylist, element_ptr_t element, int index);
// Inserts a new mylist node containing 'element' in 'mylist' at position 'index'  and returns a pointer to the new mylist.
// Remark: the first mylist node has index 0.
// If 'index' is 0 or negative, the mylist node is inserted at the start of 'mylist'. 
// If 'index' is bigger than the number of elements in 'mylist', the mylist node is inserted at the end of 'mylist'.
// Returns NULL if memory allocation failed and mylist_errno is set to mylist_MEMORY_ERROR 

mylist_ptr_t mylist_remove_at_index( mylist_ptr_t mylist, int index);
// Removes the mylist node at index 'index' from 'mylist'. NO free() is called on the element pointer of the mylist node. 
// If 'index' is 0 or negative, the first mylist node is removed. 
// If 'index' is bigger than the number of elements in 'mylist', the last mylist node is removed.
// If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR (to see the difference with removing the last element from a mylist)

mylist_ptr_t mylist_free_at_index( mylist_ptr_t mylist, int index);
// Deletes the mylist node at index 'index' in 'mylist'. 
// A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer. 
// If 'index' is 0 or negative, the first mylist node is deleted. 
// If 'index' is bigger than the number of elements in 'mylist', the last mylist node is deleted.
// If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR (to see the difference with freeing the last element from a mylist)

mylist_node_ptr_t mylist_get_reference_at_index( mylist_ptr_t mylist, int index );
// Returns a reference to the mylist node with index 'index' in 'mylist'. 
// If 'index' is 0 or negative, a reference to the first mylist node is returned. 
// If 'index' is bigger than the number of mylist nodes in 'mylist', a reference to the last mylist node is returned. 
// If the mylist is empty, NULL is returned.

element_ptr_t mylist_get_element_at_index( mylist_ptr_t mylist, int index );
// Returns the mylist element contained in the mylist node with index 'index' in 'mylist'. 
// If 'index' is 0 or negative, the element of the first mylist node is returned. 
// If 'index' is bigger than the number of elements in 'mylist', the element of the last mylist node is returned.
// If the mylist is empty, NULL is returned.

int mylist_get_index_of_element( mylist_ptr_t mylist, element_ptr_t element );
// Returns an index to the first mylist node in 'mylist' containing 'element'.  
// If 'element' is not found in 'mylist', -1 is returned.

void mylist_print( mylist_ptr_t mylist );
// for testing purposes: print the entire mylist on screen

#ifdef mylist_EXTRA
  mylist_ptr_t mylist_insert_at_reference( mylist_ptr_t mylist, element_ptr_t element, mylist_node_ptr_t reference );
  // Inserts a new mylist node containing 'element' in the 'mylist' at position 'reference'  and returns a pointer to the new mylist. 
  // If 'reference' is NULL, the element is inserted at the end of 'mylist'.

  mylist_ptr_t mylist_insert_sorted( mylist_ptr_t mylist, element_ptr_t element );
  // Inserts a new mylist node containing 'element' in the sorted 'mylist' and returns a pointer to the new mylist. 
  // The 'mylist' must be sorted before calling this function. 
  // The sorting is done in ascending order according to a comparison function.  
  // If two members compare as equal, their order in the sorted array is undefined.

  mylist_ptr_t mylist_remove_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Removes the mylist node with reference 'reference' in 'mylist'. 
  // NO free() is called on the element pointer of the mylist node. 
  // If 'reference' is NULL, the last mylist node is removed.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR

  mylist_ptr_t mylist_free_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Deletes the mylist node with position 'reference' in 'mylist'. 
  // A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer. 
  // If 'reference' is NULL, the last mylist node is deleted.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR

  mylist_ptr_t mylist_remove_element( mylist_ptr_t mylist, element_ptr_t element );
  // Finds the first mylist node in 'mylist' that contains 'element' and removes the mylist node from 'mylist'. 
  // NO free() is called on the element pointer of the mylist node.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR
  
  mylist_node_ptr_t mylist_get_first_reference( mylist_ptr_t mylist );
  // Returns a reference to the first mylist node of 'mylist'. 
  // If the mylist is empty, NULL is returned.

  mylist_node_ptr_t mylist_get_last_reference( mylist_ptr_t mylist );
  // Returns a reference to the last mylist node of 'mylist'. 
  // If the mylist is empty, NULL is returned.

  mylist_node_ptr_t mylist_get_next_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Returns a reference to the next mylist node of the mylist node with reference 'reference' in 'mylist'. 
  // If the next element doesn't exists, NULL is returned.

  mylist_node_ptr_t mylist_get_previous_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Returns a reference to the previous mylist node of the mylist node with reference 'reference' in 'mylist'. 
  // If the previous element doesn't exists, NULL is returned.

  element_ptr_t mylist_get_element_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Returns the element pointer contained in the mylist node with reference 'reference' in 'mylist'. 
  // If 'reference' is NULL, the element of the last element is returned.

  mylist_node_ptr_t mylist_get_reference_of_element( mylist_ptr_t mylist, element_ptr_t element );
  // Returns a reference to the first mylist node in 'mylist' containing 'element'. 
  // If 'element' is not found in 'mylist', NULL is returned.

  int mylist_get_index_of_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference );
  // Returns the index of the mylist node in the 'mylist' with reference 'reference'. 
  // If 'reference' is NULL, the index of the last element is returned.

  mylist_ptr_t mylist_free_element( mylist_ptr_t mylist, element_ptr_t element );
  // Finds the first mylist node in 'mylist' that contains 'element' and deletes the mylist node from 'mylist'. 
  // A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer.  
#endif

#endif  //mylist_H_

