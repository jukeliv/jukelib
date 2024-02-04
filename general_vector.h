#include <stdlib.h>

/*
  # Generic Vector by Máximo Carrasco
   A simple single-header library that implements a dynamic array in C
  # Usage
   just include the header, it is a single-header library so it will just work by
   including `vector.h` in your .c file. i.e :
   ```c
   #include <stdio.h>
   #include "vector.h"

   // VECTOR_TEMPLATE takes as input a data-type and generates a new struct
   // that struct would be: vector_yourDataType
   VECTOR_TEMPLATE(int)

   int main(void) {
     // VECTOR_INIT takes as input a data-type and just sets up your dynamic array
     // defining all the necesary function pointers and values for the struct
     vector_int vector = VECTOR_INIT(int);
     vector.push(&vector, 34);
     vector.push(&vector, 35);

     printf("%d", vector.pop(&vector)+vector.pop(&vector));

     return 0;
   }
   ```
  # Library functions
   void vector.push(vector_T* vector, T element):
    pushes an element to the top of the vector
   T vector.pop(vector_T* vector):
    pops the top value of the vector and returns it
   T vector.at(vector_T* vector, size_t index):
    if the index is greater than the size of the vector it returns (T)0
    else it returns the value at that index in the vector
   void vector.drop(vector_T* vector, size_t index):
    drops the value at that index ( if it is outside bounds it does nothing )
   void vector.insert(vector_T* vector, size_t index, T element):
    inserts the element at that index in the vector
*/

#ifndef GENERAL_VECTOR_H_
#define GENERAL_VECTOR_H_

#define __VECTOR_PUSH(T) \
  void __vector_##T##_push_element(vector_##T* vector, T element) { \
    if (vector->cap <= vector->len) { \
      vector->cap *= 2; \
      vector->content = (T*)realloc(vector->content, vector->cap*sizeof(T)); \
    } \
    vector->content[vector->len++] = element; \
  }

#define __VECTOR_POP(T) \
  T __vector_##T##_pop_element(vector_##T* vector) { \
    if (!vector->len) { \
      return (T)0; \
    } \
    return vector->content[--vector->len]; \
  }

#define __VECTOR_AT(T) \
  T __vector_##T##_element_at(vector_##T* vector, size_t index) { \
    if (index >= vector->len) { \
      return (T)0; \
    } \
    return vector->content[index]; \
  }

#define VECTOR_DROP(T) \
  void __vector_##T##_drop_element(vector_##T* vector, size_t index) { \
    for (size_t i = index; i < vector->len; ++i) { \
      vector->content[i] = vector->content[i+1]; \
    } \
    --vector->len; \
  }

#define VECTOR_INSERT(T) \
  void __vector_##T##_insert_element(vector_##T* vector, size_t index, T element) { \
    if (vector->cap <= vector->len) { \
      vector->cap *= 2; \
      vector->content = (T*)realloc(vector->content, vector->cap*sizeof(T)); \
    } \
    for (size_t i = vector->len; i > index; --i) { \
      vector->content[i] = vector->content[i-1]; \
    } \
    vector->content[index] = element; \
    ++vector->len; \
  }
#define VECTOR_TEMPLATE(T) \
  typedef struct vector_##T {\
    T* content; \
    size_t len; \
    size_t cap; \
    void (*push)  (struct vector_##T*, T); \
    T (*pop)      (struct vector_##T*);\
    T (*at)       (struct vector_##T*, size_t); \
    void (*drop)  (struct vector_##T*, size_t); \
    void (*insert)(struct vector_##T*, size_t, T); \
  } vector_##T;\
  __VECTOR_PUSH(T)  \
  __VECTOR_POP(T)   \
  __VECTOR_AT(T)    \
  __VECTOR_DROP(T)  \
  __VECTOR_INSERT(T)\

#define VECTOR_INIT(T) (vector_##T){ \
  .content = malloc(sizeof(T)), \
  .cap = 1, .len = 0, \
  .push =   &__vector_##T##_push_element, \
  .pop =    &__vector_##T##_pop_element, \
  .at =     &__vector_##T##_element_at, \
  .drop =   &__vector_##T##_drop_element, \
  .insert = &vector_##T##_insert_element \
  }
#endif
