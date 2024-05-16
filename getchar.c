#include <stdio.h>

int main(void) {
	// so getchar gets input from stdin
	// stdin can be from keyboard or a file like ./getchar < file.txt
	// or from other program otherprogram | ./getchar
	// actually how it works is, first if i call getchar
	// then it will look for input in stdin
	// if there is no input from stdin then it waits for the input
	// when it gets the input, then it will read the value from first
	// pointer and the pointer moves to next character
	// Thus c here will have first pointer character integer
	// and d will have next character
	int c = getchar();
	int d = getchar();
	int e = getchar();
	int f = getchar();

	/*
	 echo "h" | ./a.out
	 will result in
	 ```
	 c val: 104
	 h
	 d val: 10
	 
	 
	 e val: -1
	 
	 f val: -1
	 ```

	 from the result it seems that, after h echo sends another character in stdin together
	 with h where the other character has integer 10 value.

	 then lastly everything else is EOF as that is end of the stdin input

	 there is difference between this and while execution which is confusing me:
	 If getchar works like i think which is, it is just reading the character from
	 stdin input and if not input then it waits for the stdin.

	 whereas in while((c = getchar()) != EOF) example, it continuosly waits from stdin
	 like for the first getchar when no stdin input provided, it waits for stdin
	 When we give it stdin from keyboard, then above while loop will run untill it
	 encounters EOF. But when it completes the input then it should encounter the
	 EOF char and then it should get out of the while loop but it doesn't and again
	 waits for stdin input.

	 Or another theory is that, in keyboard input to stdin, the last character is not
	 EOF, so after reading the input till the last, it agains waits for stdin input
	 as it didn't encounter EOF character.
	*/

	printf("\nc val: %d\n", c);
	// putchar i think just converts the character integer to character and then
	// outputs to stdout
	putchar(c);
	printf("\nd val: %d\n", d);
	putchar(d);
	printf("\ne val: %d\n", e);
	putchar(e);
	printf("\nf val: %d\n", f);
	putchar(f);

	return 0;
}
