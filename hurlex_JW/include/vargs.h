#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

typedef __builtin_va_list va_list;

#define va_start(ap, last)	(__builtin_va_start(ap, last))
#define va_arg(ap, type)	(__builtin_va_arg(ap, type))
#define va_end(ap)

/*
#define va_list char*
#define va_start(ap, last)	(ap=(va_list)&last+sizeof(last))
#define va_arg(ap, type)	(*(type*)( (ap+=sizeof(type))-sizeof(type) )) // 更新ap
#define va_end(ap)			(ap=(va_list)NULL)
*/

#endif
