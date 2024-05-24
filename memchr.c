/*
Implementation copied from memchr.c at glibc source code repo
*/

#include <stdlib.h>
#include <stdint.h>

/*
What this effectively do is getting the base which is some number to nearest
multiple of size from down. So if base = 17 and size = 8, then it will give
value 16. The reason for and operation because:
8 = b'1000' and 17 = b'10001' but since 8 is unsigned integer, then doing
negation of it will create a two complement of number by doing this:

the reason for 0xffffffff cause it is the maximum value the unsigned int
can have and for denoting negative number we go like this.
Refer this article: 
[Two complement with unsigned int](https://medium.com/@murmuring/binary-representation-unsigned-and-2s-complement-f8b3afa4a3ef)

0xffffffff - 0x8 + 1 = 0xfffffff8

Here, thus -(size) will become 0xfffffff8

IN hex form, it isn't clear why it creates a mask but in binary form:
					   | here towards right are insignificant of 8 binary
0xffffff8 => b'11111111111111111111111111111000'

See how it makes a mask where digits above significant part of size which is above
1000 in binary of 8 are 1 and less significant are 0's.

Another example: let's take 16 which is 0x10 ( notice that most of the number like 16
32, 48, .. etc are 10 muliple like in hex which is 0x10, 0x20, 0x30, ... etc )

16 => b'10000'

Now taking negation of above,

0xffffffff - 0x10 + 1 = 0xfffffff0 => b'11111111111111111111111111110000'

Here, we are creating mask because all the less significant digits below will by zero
as in and operation any with 0 will be 0 and above will be 0 or 1 depending upon input.

If input or base is 9 -> b'1001' & b'1000' => b'1000' which is 8 which is closest multiple of 8
any number below 8 as base will give 0 which is closest multiple to the base from down

Going back: 

17 -> b'10001' and bit mask created by negation -> b'11000'
b'10001' & b'11000' => b'10000' which is 16 in base 10 number system which is closest
multiple of 8 to the base from below

This AND operation to get multiple makes sense as left most 1 is considered and rest are zeroed
This above statement is important as this trick will only for for those which have binary representation
in form of b'1 and rest are 0's'. Checked this method with size = 10 but got result for nearest neighbour
of multiple of 2.

Lastly __typeof__ as typecasting size to base type to avoid type related issue's.
*/
// copied from libc-pointer-arith.h from glibc source code
#define ALIGN_DOWN(base, size) ((base) & -((__typeof__ (base)) (size)))

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
#define PTR_ALIGN_DOWN(base, size) \
	((__typeof__ (base)) ALIGN_DOWN((uintptr_t) (base), (size)))

typedef unsigned long int uli;
uli repeat_bytes(unsigned char c_in);

int main(void) {
	return 0;
}

void *mine_memchr(void *s, int c_in, size_t n) {
	if (n == 0) {
		return NULL;
	}

	uli *word_ptr;

	return NULL;
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
	return (((uli) - 1) / 0xff) * c_in;
}
