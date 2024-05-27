/*
Implementation copied from memchr.c at glibc source code repo
*/

#include <endian.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
What this effectively do is getting the base which is some number to nearest
multiple of size from down. So if base = 17 and size = 8, then it will give
value 16. The reason for and operation because:
8 = b'1000' and 17 = b'10001' but since 8 is unsigned integer, then doing
negation of it will create a two complement of number by doing this:

the reason for 0xffffffff cause it is the maximum value the unsigned int
can have and for denoting negative number we go like this.
Refer this article:
[Two complement with unsigned
int](https://medium.com/@murmuring/binary-representation-unsigned-and-2s-complement-f8b3afa4a3ef)

0xffffffff - 0x8 + 1 = 0xfffffff8

Here, thus -(size) will become 0xfffffff8

IN hex form, it isn't clear why it creates a mask but in binary form:
                                           | here towards right are
insignificant of 8 binary 0xffffff8 => b'11111111111111111111111111111000'

See how it makes a mask where digits above significant part of size which is
above 1000 in binary of 8 are 1 and less significant are 0's.

Another example: let's take 16 which is 0x10 ( notice that most of the number
like 16 32, 48, .. etc are 10 muliple like in hex which is 0x10, 0x20, 0x30, ...
etc )

16 => b'10000'

Now taking negation of above,

0xffffffff - 0x10 + 1 = 0xfffffff0 => b'11111111111111111111111111110000'

Here, we are creating mask because all the less significant digits below will by
zero as in and operation any with 0 will be 0 and above will be 0 or 1 depending
upon input.

If input or base is 9 -> b'1001' & b'1000' => b'1000' which is 8 which is
closest multiple of 8 any number below 8 as base will give 0 which is closest
multiple to the base from down

Going back:

17 -> b'10001' and bit mask created by negation -> b'11000'
b'10001' & b'11000' => b'10000' which is 16 in base 10 number system which is
closest multiple of 8 to the base from below

This AND operation to get multiple makes sense as left most 1 is considered and
rest are zeroed This above statement is important as this trick will only for
for those which have binary representation in form of b'1 and rest are 0's'.
Checked this method with size = 10 but got result for nearest neighbour of
multiple of 2.

Lastly __typeof__ as typecasting size to base type to avoid type related
issue's.
*/
// copied from libc-pointer-arith.h from glibc source code
#define ALIGN_DOWN(base, size) ((base) & -((__typeof__(base))(size)))

/*
Here, we are aligning the pointer to nearest neighbour of multiple of size value
Here, first we are typecasting the base to uintptr_t which is unsigned long int

The reason for type casting as we can't do bit operation which we do above as
AND with void * value. It is represented with hex while printing but it is not a
number so we typecast it to the number and then do align_down and at last we
typecast result back to base original type which is void * variant like double
*, etc.

UPDATED:
Bro there is nothing special to uintptr_t, it is just that the max value of
pointer a 64 bit system can have is max 0xffffffffffffffff pointer hex. What i
mean is, this is the max any pointer variable the value can have. If a pointer
points to 0xffffffffffffffff then the system has about 18446744073709551615
memory location which is absurd. Thus to represent this in integer we are using
unsigned long int which is sufficient to show 64 bits. uintptr_t is just typedef
of unsigned long int. Don't do shamanism, understand stuff bro.

But again, above stuff when unsigned long is of 64 bits, here we are not
specifying the literal value of bits cause some system have different amount of
bits to unsigned long int.

----
Another thing i found is that uintptr_t is good type for typecasting pointer to
integers as we can reliably typecast it back to pointer value.
[src](https://stackoverflow.com/a/1846648)
*/
#define PTR_ALIGN_DOWN(base, size)                                             \
  ((__typeof__(base))ALIGN_DOWN((uintptr_t)(base), (size)))

typedef unsigned long int uli;
void *mine_memchr(void *s, int c_in, size_t n);
uli repeat_bytes(unsigned char c_in);
uli _find_zeros_all(uli word);
uli _find_zeros_low(uli word);
uli shift_bits(uli word, uintptr_t s);
int ctz(uli word);
int clz(uli word);
int index_first(uli word);

int main(void) {
  char *string =
      "MSSubClass,MSZoning,LotFrontage,LotArea,Street,Alley,LotShape,"
      "LandContour,Utilities,LotConfig,LandSlope,Neighborhood,Condition1,"
      "Condition2,BldgType,HouseStyle,OverallQual,OverallCond,YearBuilt,"
      "YearRemodAdd,RoofStyle,RoofMatl,Exterior1st,Exterior2nd,MasVnrType,"
      "MasVnrArea,ExterQual,ExterCond,Foundation,BsmtQual,BsmtCond,"
      "BsmtExposure,BsmtFinType1,BsmtFinSF1,BsmtFinType2,BsmtFinSF2,BsmtUnfSF,"
      "TotalBsmtSF,Heating,HeatingQC,CentralAir,Electrical,FirstFlrSF,"
      "SecondFlrSF,LowQualFinSF,GrLivArea,BsmtFullBath,BsmtHalfBath,FullBath,"
      "HalfBath,BedroomAbvGr,KitchenAbvGr,KitchenQual,TotRmsAbvGrd,Functional,"
      "Fireplaces,FireplaceQu,GarageType,GarageFinish,GarageCars,GarageArea,"
      "GarageQual,GarageCond,PavedDrive,WoodDeckSF,OpenPorchSF,EnclosedPorch,"
      "Threeseasonporch,ScreenPorch,PoolArea,PoolQC,Fence,MiscFeature,MiscVal,"
      "MoSold,YearSold,SaleType,SaleCondition,SalePrice";

  int sl = strlen(string);
  for (char *temp = string;
       (temp = mine_memchr(temp, ',', (sl - (temp - string)))); temp++) {
    printf("%c\t", *(temp + 1));
  }

  return 0;
}

void *mine_memchr(void *s, int c_in, size_t n) {
  if (n == 0) {
    return NULL;
  }
  // it is executing for all char meaning it returns
  // for every char

  // explained in csv_reader.c in csv_reader/ about why this
  // and what word means here
  uli *word_ptr = PTR_ALIGN_DOWN(s, sizeof(uli));
  uintptr_t s_int = (uintptr_t)s;

  char *last_byte =
      (char *)(((s_int + n - 1) > UINTPTR_MAX) ? UINTPTR_MAX : (s_int + n - 1));
  uli *last_word_ptr = (void *)PTR_ALIGN_DOWN(last_byte, sizeof(uli));

  uli repeated_bytes = repeat_bytes(c_in);
  uli word = *word_ptr;

  // will return 0x80 for those where word and repeated_bytes match
  // refer csv_reader.c for more explanation
  uli res = shift_bits(_find_zeros_all(word ^ repeated_bytes), s_int);

  if (res != 0) {
    char *ret = (char *)(s + index_first(res));
    // don't need to return NULL as if ret > last_byte
    // then it means the current word ptr is last word ptr
    // which is handled below
    if (ret <= last_byte) {
      // it returns everytime from here
      return ret;
    }
  }

  // as if res didn't have any char match and since
  // it is last word. Thus returning NULL indicating
  // there are no position in string which match the char.
  if (word_ptr == last_word_ptr) {
    return NULL;
  }

  word = *++word_ptr;
  while (word_ptr != last_word_ptr) {
    uli res = _find_zeros_low(word ^ repeated_bytes);
    // again there is difference between this and original source code
    // where they defined index_first_eq which according to __BYTE_ORDER
    // does either _find_zeros_low or _find_zeros_all and then use that
    // to find index_first. I kind of not getting it as what is difference
    // between _find_zeros_all and _find_zeros_low.
    // will see in future
    if (res != 0) {
      char *ret = (char *)(((char *)word_ptr) + index_first(res));
      return ret;
    }
    word = *++word_ptr;
  }

  res = _find_zeros_low(word ^ repeated_bytes);
  // same as above, used index_first_eq here than directly
  // _find_zeros_low
  if (res != 0) {
    char *ret = (char *)(((char *)word_ptr) + index_first(res));
    if (ret <= last_byte) {
      return ret;
    }
  }

  return NULL;
}

int ctz(uli word) {
  if (sizeof(uli) == sizeof(unsigned long int)) {
    return __builtin_ctzl(word);
  } else {
    return __builtin_ctzll(word);
  }
}

int clz(uli word) {
  if (sizeof(uli) == sizeof(unsigned long int)) {
    return __builtin_clzl(word);
  } else {
    return __builtin_clzll(word);
  }
}

int index_first(uli word) {
  if (__BYTE_ORDER == __LITTLE_ENDIAN) {
    return ctz(word) / CHAR_BIT;
  } else {
    return clz(word) / CHAR_BIT;
  }
}

// refer csv_reader.c for more explanation
uli shift_bits(uli word, uintptr_t s) {
  if (__BYTE_ORDER == __LITTLE_ENDIAN) {
    return word >> (CHAR_BIT * (s % sizeof(uli)));
  } else {
    return word << (CHAR_BIT * (s % sizeof(uli)));
  }
}

// refer csv_reader.c for more explanation
uli _find_zeros_all(uli word) {
  uli m = repeat_bytes(0x7f);
  return ~(((word & m) + m) | word | m);
}

// i don't get how it is different from above
// as checked for all result and it gives same value
// as above. Refer string-fza in glibc source code in future
// if i get some idea or something
uli _find_zeros_low(uli word) {
  uli sub = repeat_bytes(0x01);
  uli m = repeat_bytes(0x80);
  return (word - sub) & ~word & m;
}

uli repeat_bytes(unsigned char c_in) {
  /*
  here -1 the (uli) is typecasted to max
  amount the variable of that type can take
  since is uli variable size is 8 bytes
  thus it uses 64 bits

  double total_bits_uli = sizeof(op_t) * 8;
  unsigned long int max_uli = powl(2, total_bits_uli) - 1;

  so (uli) - 1 => will return 0xffffffffffffffff

  a total of 16 f's as 2 ff is one byte

  Now since we want repeated bytes, we want this
  0x0101010101010101

  as any hex times above is repeated version of the hex
  For example: 0x010101 * 0xce => 0xcecece

  This works as proof can be done with similar arithmetic operation
  we have done with base 10 numbers

  CHATGPT:
  The key reason base 10 arithmetic methods work in other bases is that
  arithmetic operations are fundamentally about combining quantities. The
  process of carrying over, shifting positions, and summing partial
  results applies universally across bases because they are rooted in the
  positional value system.

  Now to get 01 pattern, let's divide 0xffff.. with 0xff as we can see that
  dividng ff | ffffff -> first one'za * ff = ff
  so 1 in quotient, now since we have to take out two
  number to divide we add 0 above thus 10
  again for ff -> 1010 this is quotient
  and now we need to divide ff lastly thus 10101

  Now lastly we are multiplying with the char that which we want to be repeated
  This function will repeat the char for that number of bytes of uli
  */
  return (((uli)-1) / 0xff) * c_in;
}
