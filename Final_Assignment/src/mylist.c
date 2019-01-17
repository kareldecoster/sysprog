#include "../include/mylist.h"
#include <stdlib.h>


typedef void (*copy_element_func)(element_ptr_t *dest_element, element_ptr_t src_element);
typedef void (*free_element_func)(element_ptr_t *element);
typedef int (*compare_element_func)(element_ptr_t x, element_ptr_t y);
typedef void (*print_element_func)(element_ptr_t element);

struct mylist{
	mylist_node_t* first_node;
	mylist_node_t* last_node;
	int size;
	copy_element_func copy_element;
	free_element_func free_element;
	compare_element_func compare_element;
	print_element_func print_element;
};

struct mylist_node{
	element_ptr_t element;
	mylist_node_t* previous;
	mylist_node_t* next;
};

// Returns a pointer to a newly-allocated mylist.
// Returns NULL if memory allocation failed and mylist_errno is set to mylist_MEMORY_ERROR 
mylist_ptr_t mylist_create(
			  void (*element_copy)(element_ptr_t *dest_element, element_ptr_t src_element),
			  void (*element_free)(element_ptr_t *element),
			  int (*element_compare)(element_ptr_t x, element_ptr_t y),
			  void (*element_print)(element_ptr_t element)
			){
	mylist_ptr_t mymylist = malloc(sizeof(mylist_t));
	mylist_errno = mylist_NO_ERROR;
	if(mymylist==NULL){
		mylist_errno = mylist_MEMORY_ERROR;
		return mymylist;
		
	}
	mymylist->copy_element = element_copy;
	mymylist->free_element = element_free;
	mymylist->compare_element = element_compare;
	mymylist->print_element = element_print;
	mymylist->size = 0;
	mymylist->first_node = NULL;	
	mymylist->last_node = NULL;
			
	return mymylist;	
			
}

// Every mylist node and node element of the mylist needs to be deleted (free memory)
// The mylist itself also needs to be deleted (free all memory) and set to NULL
void mylist_free( mylist_ptr_t* mylist ){
	mylist_errno = mylist_NO_ERROR;
	if(mylist != NULL){
		//First free all the elements.
		mylist_node_ptr_t node = (*mylist)->first_node;
		mylist_node_ptr_t next;
		while(node != NULL){
			(*mylist)->free_element(&(node->element));
			node->element = NULL;
			node = node->next;
			
		}
		//Second free all the nodes.
		node = (*mylist)->first_node;
		while(node != NULL){
			node->previous = NULL;
			next = node->next;
			node->next = NULL;
			free(node);
			node = next;
		
		}
		//Finally, free the mylist.
		free(*mylist);
		*mylist = NULL;

	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		
	}
}

// Returns the number of elements in 'mylist'.
int mylist_size( mylist_ptr_t mylist ){
	mylist_errno = mylist_NO_ERROR;
	if(mylist !=NULL){
		int i = mylist->size;
		return i;
		
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return 0;
		
	}
}

// Inserts a new mylist node containing 'element' in 'mylist' at position 'index'  and returns a pointer to the new mylist.
// Remark: the first mylist node has index 0.
// If 'index' is 0 or negative, the mylist node is inserted at the start of 'mylist'. 
// If 'index' is bigger than the number of elements in 'mylist', the mylist node is inserted at the end of 'mylist'.
// Returns NULL if memory allocation failed and mylist_errno is set to mylist_MEMORY_ERROR 
mylist_ptr_t mylist_insert_at_index( mylist_ptr_t mylist, element_ptr_t element, int index){
	mylist_errno = mylist_NO_ERROR;
	if(mylist != NULL)
	{
		mylist_node_ptr_t new = malloc(sizeof(mylist_node_t));
		if(new==NULL){
			mylist_errno = mylist_MEMORY_ERROR;
			return mylist;
		}
		if(mylist->size == 0){
			new->next = NULL;
			new->previous=NULL;
			mylist->first_node = new;
			mylist->last_node = new;
			mylist->copy_element(&(new->element), element);
			
			(mylist->size)++;
			return mylist;
			
		}
		if(index<=0){
		//Insert at the beginning.
			new->previous = NULL;
			new->next = mylist->first_node;
			mylist->copy_element(&(new->element), element);
			(mylist->first_node)->previous = new;
			mylist->first_node = new;
			
			(mylist->size)++;
			return mylist;
		
		}else{
			if(index >=((mylist->size)-1)){
			//Insert at the end.
				new->next = NULL;
				new->previous = mylist->last_node;
				mylist->copy_element(&(new->element), element);	
				(mylist->last_node)->next = new;
				mylist->last_node = new;
				
				(mylist->size)++;
				return mylist;
					
			}else{
			//Insert in the middle somewhere.
				int i = 0;
				mylist_node_ptr_t node = mylist->first_node;
				for(i=0;i<index-1;i++){
					node = node->next;
				}
				new->next = node->next;
				new->previous = node;
				node->next = new;
				(new->next)->previous = new;
				mylist->copy_element(&(new->element),element);
				
				(mylist->size)++;
				return mylist;
			}
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
	
	}
	return mylist;

}

// Removes the mylist node at index 'index' from 'mylist'. NO free() is called on the element pointer of the mylist node. 
// If 'index' is 0 or negative, the first mylist node is removed. 
// If 'index' is bigger than the number of elements in 'mylist', the last mylist node is removed.
// If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR (to see the difference with removing the last element from a mylist)
mylist_ptr_t mylist_remove_at_index( mylist_ptr_t mylist, int index){
	mylist_errno = mylist_NO_ERROR;
	if(mylist!=NULL){
		if(mylist->size>0){
			mylist_node_ptr_t target;
			if(index<=0){
				//Remove the first mylist node from te mylist.
				index = 0;
			
			}else{
				if(index>=(mylist->size)){
					//Remove the last mylist node.
					index = mylist->size - 1;
					
				}	
			}	
			target = mylist_get_reference_at_index(mylist, index );
			if(target == mylist->first_node){
				mylist->first_node = target->next;
				
			}else{
				(target->previous)->next = target->next;
			
			}
			if(target == mylist->last_node){
				mylist->last_node = target->previous;
				
			}else{
				(target->next)->previous = target->previous;
			
			}
			//Remove the target node, but do not free() the element.
			target->next = NULL;
			target->previous = NULL;
			target->element = NULL;
			free(target);
			mylist->size = mylist->size - 1;
		
			return mylist;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return mylist;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return mylist;
		
	}
}

// Deletes the mylist node at index 'index' in 'mylist'. 
// A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer. 
// If 'index' is 0 or negative, the first mylist node is deleted. 
// If 'index' is bigger than the number of elements in 'mylist', the last mylist node is deleted.
// If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR (to see the difference with freeing the last element from a mylist).
mylist_ptr_t mylist_free_at_index( mylist_ptr_t mylist, int index){
	mylist_errno = mylist_NO_ERROR;
	if(mylist!=NULL){
		if(mylist->size>0){
			mylist_node_ptr_t target;
			if(index<=0){
				//Remove the first mylist node from te mylist.
				index = 0;
		
			}else{
				if(index>=(mylist->size)){
					//Remove the last mylist node.
					index = mylist->size - 1;
				
				}	
			}	
			target = mylist_get_reference_at_index(mylist, index );
			if(target == mylist->first_node){
				mylist->first_node = target->next;
			
			}else{
				(target->previous)->next = target->next;
		
			}
			if(target == mylist->last_node){
				mylist->last_node = target->previous;
			
			}else{
				(target->next)->previous = target->previous;
		
			}
			//Remove the target node, but do not free() the element.
			target->next = NULL;
			target->previous = NULL;
			mylist->free_element(&(target->element));
			target->element = NULL;
			free(target);
			mylist->size = mylist->size - 1;
	
			return mylist;
				
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return mylist;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return mylist;
		
	}
}

// Returns a reference to the mylist node with index 'index' in 'mylist'. 
// If 'index' is 0 or negative, a reference to the first mylist node is returned. 
// If 'index' is bigger than the number of mylist nodes in 'mylist', a reference to the last mylist node is returned. 
// If the mylist is empty, NULL is returned.
mylist_node_ptr_t mylist_get_reference_at_index( mylist_ptr_t mylist, int index ){
	mylist_node_ptr_t node = NULL;
	mylist_errno = mylist_NO_ERROR;
	if(mylist!=NULL){
		if(mylist->size>0){
			int i;
			mylist_node_ptr_t node = mylist->first_node;
			if(index<0){
				index = 0;
				
			}
			if(index>(mylist->size-1)){
				index = (mylist->size -1);
				
			}
			for(i=0;i<index;i++){
				node = node->next;
				
			}
			return node;
			
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
			
		}	
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return node;
		
	}
}

// Returns the mylist element contained in the mylist node with index 'index' in 'mylist'. 
// If 'index' is 0 or negative, the element of the first mylist node is returned. 
// If 'index' is bigger than the number of elements in 'mylist', the element of the last mylist node is returned.
// If the mylist is empty, NULL is returned.
element_ptr_t mylist_get_element_at_index( mylist_ptr_t mylist, int index ){
	mylist_errno = mylist_NO_ERROR;
	element_ptr_t target = NULL;
	if(mylist!=NULL){
		if(mylist->size>0){
			mylist_node_ptr_t node = mylist_get_reference_at_index(mylist, index);
			target = node->element;
			return target;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return target;
			
		}
	}else{	
		mylist_errno = mylist_MEMORY_ERROR;
		return target;
	
	}
}

// Returns an index to the first mylist node in 'mylist' containing 'element'.  
// If 'element' is not found in 'mylist', -1 is returned.
int mylist_get_index_of_element( mylist_ptr_t mylist, element_ptr_t element ){
	mylist_errno = mylist_NO_ERROR;
	if(mylist!=NULL){
		if(mylist->size>0){
			int index = 0;
			mylist_node_ptr_t node = mylist->first_node;
			while(node!=NULL){
				if(mylist->compare_element(element,node->element)==0){
					return index;
				}
				node = node->next;
				index++;
			}
			return -1;
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return -1;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return -1;
		
	}
}

// for testing purposes: print the entire mylist on screen
void mylist_print( mylist_ptr_t mylist ){
	mylist_errno = mylist_NO_ERROR;
	if(mylist!=NULL){
		if(mylist->size >0){
			mylist_node_ptr_t node = mylist->first_node;
			while(node!=NULL){
				mylist->print_element(node->element);
				node = node->next;
				
			}
		}else{
			mylist_errno=mylist_EMPTY_ERROR;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		
	}
}

// Inserts a new mylist node containing 'element' in the 'mylist' at position 'reference'  and returns a pointer to the new mylist. 
// If 'reference' is NULL, the element is inserted at the end of 'mylist'.
mylist_ptr_t mylist_insert_at_reference( mylist_ptr_t mylist, element_ptr_t element, mylist_node_ptr_t reference ){
	if(mylist !=NULL){
		mylist_node_ptr_t new = malloc(sizeof(mylist_node_t));
		if(new==NULL){
			mylist_errno = mylist_MEMORY_ERROR;
			return NULL;
			
		}
		int index;
		if(reference == NULL){
		//Insert the new node at the back.
			index = mylist->size;
			
		}else{
		//Insert the new node at index 'index'.
			index = mylist_get_index_of_element(mylist, element );
			
		}
		mylist_insert_at_index( mylist, element, index);
		return mylist;
		
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
	}
}

// Inserts a new mylist node containing 'element' in the sorted 'mylist' and returns a pointer to the new mylist. 
// The 'mylist' must be sorted before calling this function. 
// The sorting is done in ascending order according to a comparison function.  
// If two members compare as equal, their order in the sorted array is undefined.
mylist_ptr_t mylist_insert_sorted( mylist_ptr_t mylist, element_ptr_t element ){
	if(mylist!= NULL){
		int index = 0;
		if(mylist->size==0){
			mylist = mylist_insert_at_index(mylist, element, 0);
			return mylist;
		
		}else{
			for(index=0;index<=mylist->size;index++){
				if(mylist->compare_element(mylist_get_element_at_index(mylist,index),element)==1){
					mylist_insert_at_index( mylist, element, index);
					return mylist;
				}			
			}
		}

		mylist = mylist_insert_at_index( mylist, element, index);
		
		return mylist;
		
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
	
	}		
}


// Returns the index of the mylist node in the 'mylist' with reference 'reference'. 
// If 'reference' is NULL, the index of the last element is returned.
int mylist_get_index_of_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	mylist_node_ptr_t node = mylist->first_node;
	int index=0;
	if(reference == NULL){
		return ((mylist->size) - 1);
		
	}
	while(node != reference && node !=NULL){
		node = node->next;
		index++;
		
	}
	if(node == NULL){
		mylist_errno = mylist_MEMORY_ERROR;
		return -1;
		
	}
	return index;
	
}


  // Removes the mylist node with reference 'reference' in 'mylist'. 
  // NO free() is called on the element pointer of the mylist node. 
  // If 'reference' is NULL, the last mylist node is removed.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR
mylist_ptr_t mylist_remove_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	if(mylist!=NULL){
		if(mylist->size>0){
			mylist = mylist_remove_at_index( mylist, mylist_get_index_of_reference( mylist, reference ));
			
			return mylist;
			
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return mylist;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return mylist;
	
	}
}

  // Deletes the mylist node with position 'reference' in 'mylist'. 
  // A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer. 
  // If 'reference' is NULL, the last mylist node is deleted.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR
mylist_ptr_t mylist_free_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	if(mylist!=NULL){
		if(mylist->size>0){
			mylist = mylist_free_at_index( mylist, mylist_get_index_of_reference( mylist, reference ));
			
			return mylist;
			
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return mylist;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return mylist;
	
	}
}


  // Finds the first mylist node in 'mylist' that contains 'element' and removes the mylist node from 'mylist'. 
  // NO free() is called on the element pointer of the mylist node.
  // If the mylist is empty, return mylist and mylist_errno is set to mylist_EMPTY_ERROR
mylist_ptr_t mylist_remove_element( mylist_ptr_t mylist, element_ptr_t element ){
	if(mylist!=NULL){
		if(mylist->size >0){
			mylist = mylist_remove_at_index(mylist,mylist_get_index_of_element(mylist,element));
			
			return mylist;
			
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return mylist;
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return mylist;
	
	}

}

  // Returns a reference to the first mylist node of 'mylist'. 
  // If the mylist is empty, NULL is returned.
mylist_node_ptr_t mylist_get_first_reference( mylist_ptr_t mylist ){
	if(mylist!=NULL){
		if(mylist->size >0){
			return mylist->first_node;		
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}


  // Returns a reference to the last mylist node of 'mylist'. 
  // If the mylist is empty, NULL is returned.
mylist_node_ptr_t mylist_get_last_reference( mylist_ptr_t mylist ){
	if(mylist!=NULL){
		if(mylist->size >0){
			return mylist->last_node;		
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
			
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}

  // Returns a reference to the next mylist node of the mylist node with reference 'reference' in 'mylist'. 
  // If the next element doesn't exists, NULL is returned.
mylist_node_ptr_t mylist_get_next_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	if(mylist!=NULL){
		if(mylist->size < 1){
			mylist_errno = mylist_NO_ERROR;
			return reference->next;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}


  // Returns a reference to the previous mylist node of the mylist node with reference 'reference' in 'mylist'. 
  // If the previous element doesn't exists, NULL is returned.
mylist_node_ptr_t mylist_get_previous_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	if(mylist!=NULL){
		if(mylist->size < 1){
			mylist_errno = mylist_NO_ERROR;
			return reference->previous;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}


  // Returns the element pointer contained in the mylist node with reference 'reference' in 'mylist'. 
  // If 'reference' is NULL, the element of the last element is returned.
element_ptr_t mylist_get_element_at_reference( mylist_ptr_t mylist, mylist_node_ptr_t reference ){
	if(mylist!=NULL){
		if(mylist->size < 1){
			mylist_errno = mylist_NO_ERROR;
			return reference->element;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}

  // Returns a reference to the first mylist node in 'mylist' containing 'element'. 
  // If 'element' is not found in 'mylist', NULL is returned.
mylist_node_ptr_t mylist_get_reference_of_element( mylist_ptr_t mylist, element_ptr_t element ){
	if(mylist!=NULL){
		if(mylist->size < 1){
			mylist_errno = mylist_NO_ERROR;
			return mylist_get_reference_at_index(mylist,(mylist_get_index_of_element(mylist,element)));
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}

  // Finds the first mylist node in 'mylist' that contains 'element' and deletes the mylist node from 'mylist'. 
  // A free() is called on the element pointer of the mylist node to free any dynamic memory allocated to the element pointer.
mylist_ptr_t mylist_free_element( mylist_ptr_t mylist, element_ptr_t element ){
	if(mylist!=NULL){
		if(mylist->size < 1){
			mylist_errno = mylist_NO_ERROR;
			mylist = mylist_free_at_index(mylist,(mylist_get_index_of_element(mylist,element)));
			return mylist;
		
		}else{
			mylist_errno = mylist_EMPTY_ERROR;
			return NULL;
		
		}
	}else{
		mylist_errno = mylist_MEMORY_ERROR;
		return NULL;
		
	}
}

















