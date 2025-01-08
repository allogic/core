#ifndef CORE_MACROS_H
#define CORE_MACROS_H

#ifndef CORE_MAX
	#define CORE_MAX(A, B) (((A) > (B)) ? (A) : (B))
#endif // CORE_MAX

#ifndef CORE_MIN
	#define CORE_MIN(A, B) (((A) < (B)) ? (A) : (B))
#endif // CORE_MIN

#ifndef CORE_ALIGN_DOWN_BY
	#define CORE_ALIGN_DOWN_BY(VALUE, ALIGNMENT) (((uint64_t)(VALUE)) & ~(((uint64_t)(ALIGNMENT)) - 1))
#endif // CORE_ALIGN_DOWN_BY

#ifndef CORE_ALIGN_UP_BY
	#define CORE_ALIGN_UP_BY(VALUE, ALIGNMENT) ((((uint64_t)(VALUE)) + (((uint64_t)(ALIGNMENT)) - 1)) & ~(((uint64_t)(ALIGNMENT)) - 1))
#endif // CORE_ALIGN_UP_BY

#ifndef CORE_IS_BIT_SET
	#define CORE_IS_BIT_SET(VALUE, BIT) (((uint64_t)(VALUE)) & (((uint64_t)1) << (BIT)))
#endif // CORE_IS_BIT_SET

#ifndef CORE_IS_BIT_NOT_SET
	#define CORE_IS_BIT_NOT_SET(VALUE, BIT) (!(((uint64_t)(VALUE)) & (((uint64_t)1) << (BIT))))
#endif // CORE_IS_BIT_NOT_SET

#ifndef CORE_SET_BIT
	#define CORE_SET_BIT(VALUE, BIT) (((uint64_t)(VALUE)) | (((uint64_t)1) << (BIT)))
#endif // CORE_SET_BIT

#ifndef CORE_CLEAR_BIT
	#define CORE_CLEAR_BIT(VALUE, BIT) (((uint64_t)(VALUE)) & (~(((uint64_t)1) << (BIT))))
#endif // CORE_CLEAR_BIT

#ifndef CORE_TOGGLE_BIT
	#define CORE_TOGGLE_BIT(VALUE, BIT) (((uint64_t)(VALUE)) ^ (((uint64_t)1) << (BIT)))
#endif // CORE_TOGGLE_BIT

#endif // CORE_MACROS_H
