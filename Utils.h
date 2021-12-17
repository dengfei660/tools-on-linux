#ifndef _TOOS_UTILS_H_
#define _TOOS_UTILS_H_

/**
 * ROUND_UP_2:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 2.
 */
#define ROUND_UP_2(num)  (((num)+1)&~1)
/**
 * ROUND_UP_4:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 4.
 */
#define ROUND_UP_4(num)  (((num)+3)&~3)
/**
 * ROUND_UP_8:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 8.
 */
#define ROUND_UP_8(num)  (((num)+7)&~7)
/**
 * ROUND_UP_16:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 16.
 */
#define ROUND_UP_16(num) (((num)+15)&~15)
/**
 * ROUND_UP_32:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 32.
 */
#define ROUND_UP_32(num) (((num)+31)&~31)
/**
 * ROUND_UP_64:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 64.
 */
#define ROUND_UP_64(num) (((num)+63)&~63)
/**
 * ROUND_UP_128:
 * @num: integer value to round up
 *
 * Rounds an integer value up to the next multiple of 128.
 * Since: 1.4
 */
#define ROUND_UP_128(num) (((num)+127)&~127)
/**
 * ROUND_UP_N:
 * @num: integrer value to round up
 * @align: a power of two to round up to
 *
 * Rounds an integer value up to the next multiple of @align. @align MUST be a
 * power of two.
 */
#define ROUND_UP_N(num,align) ((((num) + ((align) - 1)) & ~((align) - 1)))


/**
 * ROUND_DOWN_2:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 2.
 */
#define ROUND_DOWN_2(num)  ((num)&(~1))
/**
 * ROUND_DOWN_4:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 4.
 */
#define ROUND_DOWN_4(num)  ((num)&(~3))
/**
 * ROUND_DOWN_8:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 8.
 */
#define ROUND_DOWN_8(num)  ((num)&(~7))
/**
 * ROUND_DOWN_16:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 16.
 */
#define ROUND_DOWN_16(num) ((num)&(~15))
/**
 * ROUND_DOWN_32:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 32.
 */
#define ROUND_DOWN_32(num) ((num)&(~31))
/**
 * ROUND_DOWN_64:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 64.
 */
#define ROUND_DOWN_64(num) ((num)&(~63))
/**
 * ROUND_DOWN_128:
 * @num: integer value to round down
 *
 * Rounds an integer value down to the next multiple of 128.
 * Since: 1.4
 */
#define ROUND_DOWN_128(num) ((num)&(~127))
/**
 * ROUND_DOWN_N:
 * @num: integrer value to round down
 * @align: a power of two to round down to
 *
 * Rounds an integer value down to the next multiple of @align. @align MUST be a
 * power of two.
 */
#define ROUND_DOWN_N(num,align) (((num) & ~((align) - 1)))

#endif /*_TOOS_UTILS_H_*/