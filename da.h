#ifndef __DA__
#define __DA__
typedef struct {
  char *items;
  size_t count;
  size_t capacity;
} Da_chars;


#define DA_INIT_CAP 32
#define DA_INSERT(da, el)						\
  do {									\
    if ((da)->count + 1 > (da)->capacity) {				\
      (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2; \
      (da)->items = realloc((da)->items, sizeof(*(da)->items) * (da)->capacity); \
      assert((da)->items != NULL && "You don't have enough RAM!\n");	\
    }									\
    (da)->items[(da)->count] = el;					\
    (da)->count += 1;							\
  } while(0)								

#define DA_FREE(da) free((da)->items)
#endif // __DA__
