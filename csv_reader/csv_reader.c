#include "/home/roopkumar/tid/LRN/nn_in_c/mat.h"
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// what it does is returns nearest neighbour of base which is mulitple of size
// from down. So if base = 12 and size = 8 => this will give 8
// if base = 7, size = 8 => 0
// if base = 17, size = 8 => 16
//
// Here, first converted size to that of type base as don't want problem when
// doing AND operation with -ve of it. This -ve is two complement of size.
// IDK about it, need to research!!!
#define ALIGN_DOWN(base, size) ((base) & -((__typeof__(base))(size)))

// what we are doing here, same as above which is if pointer address is let say
// 17, then with align down we return 16.
//
// But the thing is, base which is obviously a pointer, can't be directly used
// for AND or any bit operation as it is a pointer structure. To use bit
// operation, let's convert the hex value of address to integer with help of
// uintptr_t type, which is unsigned long int. This unsigned long int is enough
// to cover any address and thus reliable choice.
//
// After doing align down, we need to re type cast it to original pointer thus,
// that typeof operation
#define PTR_ALIGN_DOWN(base, size)                                             \
  ((__typeof__(base))ALIGN_DOWN((uintptr_t)(base), (size)))

// validating if string given in char_start is valid 0.00 or 0 string
#define VALIDATE_ZERO_STRING(char_start, i, temp)                              \
  int invalid_string = 0;                                                      \
  int got_dot = 0;                                                             \
  int end_posn = (buffer[i - 1] == '\r') ? (i - 1) : i;                        \
  for (int zi = char_start; zi < end_posn; zi++) {                             \
    if (buffer[zi] != '0' && buffer[zi] != '.') {                              \
      invalid_string = 1;                                                      \
      break;                                                                   \
    }                                                                          \
    if (got_dot == 1 && buffer[zi] != '0') {                                   \
      invalid_string = 1;                                                      \
      break;                                                                   \
    } else if (got_dot == 0) {                                                 \
      if (buffer[zi] == '.') {                                                 \
        got_dot = 1;                                                           \
        continue;                                                              \
      } else if (buffer[zi] != '0') {                                          \
        invalid_string = 1;                                                    \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  if (invalid_string == 1) {                                                   \
    int pps = ((char_start - 10) < 0) ? 0 : (char_start - 10);                 \
    int ppe = ((i + 10) > (bytes_read)) ? (bytes_read) : (i + 10);             \
    buffer[i] = temp;                                                          \
    for (int m = pps; m < ppe; m++) {                                          \
      if ((m >= char_start) && (m < i)) {                                      \
        fprintf(stderr, "\033[1;33m%c\033[0m", buffer[m]);                     \
        continue;                                                              \
      }                                                                        \
      fprintf(stderr, "%c", buffer[m]);                                        \
    }                                                                          \
    fprintf(stderr, "\n");                                                     \
    fprintf(stderr, "char_start: %zu, i: %zu\n", char_start, i);               \
    buffer[i] = '\0';                                                          \
    fprintf(stderr, "Couldn't convert: %s\n", &buffer[char_start]);            \
    fprintf(stderr, "--------------\n");                                       \
    time_taken = -10;                                                          \
    goto cleanup;                                                              \
  }

#define BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 200

typedef unsigned long int uli;

// copied from glibc source code repo
uli _repeat_bytes(unsigned char c_in) {
  // actually what it does is takes the char input let say 0x0a -> which is \n
  // in hex and then repeats it for total bits for unsigned long int which is 64
  // bit meaning we will have 8 repeated -> 0x0a0a0a0a0a0a0a0a
  //
  // Now there is bit trick to this, which is if we multiply 0x0101... with any
  // 1 byte hex -> then it will be repeated version like above. To prove this
  // just multiply 0x01010101... with 0x0a and we will get above. Multiplication
  // method is same as for base 10 number system as arithmetic operation are
  // general for any base number system. Thus first we multiply a to above
  // 0101.. which will make a appear at position of 1, then add extra 0 to end
  // for second multiplication and multiply 0 to above. Lastly adding the result
  // will give us above thing.
  //
  // But i can't just use 0101.. as in variable, because uli bytes requirement
  // is dependent on machine and for me it is 8 bytes. But rather than hard
  // coding, what we can do is using casting method and (uli) which normally
  // return 8 value which is 8 byte size which it takes, if we subtract one then
  // it will cast it to integer and subtract 1 from it and thus resulting in
  // 0xffffffffffffffff for me
  //
  // For 0x01.. pattern, divide by 0xff -> same method done for base 10 number.
  // ff | fff... -> 1'za then adding one more f down so add 0 to quotient and
  // lastly we will have ff | ff which is 1 more add to quotient
  //
  // Now multiply by c_in to get repeated bytes.
  return (((uli)-1) / 0xff) * c_in;
}

// here don't think word as english words, will explain it down
// where used it
// returns uli which on converted to hex will show 80 byte at those
// position where it is same char as that of c_in
uli find_similar_char(uli word, unsigned char c_in) {
  uli rby = _repeat_bytes(c_in);
  // xored as same bytes will result in 00 and rest
  // will have other value. To get this, rather than
  // taking whole one byte ce to xor, thus think
  // of e ^ any other hex number as second place of hex
  // won't really affect xored of first place of hex number
  //
  // 0xf ^ 0xf => 0x0, but 0xf ^ 0xe => 0x1
  uli xored = word ^ rby;

  // above thing is neat as then we can just count up how many
  // bytes have 00 to get how many have c_in is in the word
  // but as far i have researched, i have only able to get below
  // gcc function which uses hardware instructions to calculate number
  // of 1's in the bits given in input
  //
  // Then why not do this, let's have 0x00 as 0x80 which is in binary
  // 1000 0000 and rest other be 0x00. Thus when counted 1 it means we
  // have counted 0x00 byte
  //
  // Do do this, we have this neat bit trick

  uli mask = _repeat_bytes(0x7f);
  uli res = ~(((xored & mask) + mask) | mask | xored);
  // now let's look at it one brackett at a time, xored & mask
  // see that the mask we choose is 0x7f which in binary is
  // 0111 1111
  //
  // Now if we look at xored as each byte as one byte won't affect other
  // byte value above because of mask
  //
  // 00 to ff gets mapped to 00 to 7f
  // where 00 to 7f maps to 00 to 7f and 80 which is next to 7f as 7f + 1 = 80
  // (do arthimetic add), 80 to ff remaps to 00 to 7f
  //
  // Now second brackett, val + mask => here 00 to 7f which is val is mapped
  // to 7f to fe
  //
  // mask | xored -> here xored ranges from 00 to ff, but doing OR with 0x7f
  // maps 00 to 7f => 7f and 80 to ff => ff, expand to binary and do OR to see
  //
  // Lastly val2 | val3, where val2 ranges from 7f to fe and val3 having 7f for
  // val2 7f and ff value for rest, when done OR operation, if we look at it,
  // only 7f will be 7f and rest will be ff.
  //
  // Thus original 00 maps to 7f whereas rest maps to ff.
  //
  // Lastly we negate the result, thus 7f turns to 80 as
  // ~(0111 1111) = 1000 0000
  // and ff to 00, thus achieving 80 at those where 00 in original
  // and rest by 00

  return res;
}

/*
What it does is, 0xfaee >> 8 => 0xfa
it just shifts fa by 8 bits.
Same can be said for << which shifts towards left
Need to look at endian system, as justified where it is used

CHAR_BIT is 8 bits, s % sizeof(uli) -> will give remainder
this modulo operation is important as with this we can remove
those we don't want to consider those bytes
*/
uli shift_bits(uli word, uintptr_t s) {
  if (__BYTE_ORDER == __LITTLE_ENDIAN) {
    return word >> (CHAR_BIT * (s % sizeof(uli)));
  } else {
    return word << (CHAR_BIT * (s % sizeof(uli)));
  }
}

long buffer_read_csv(char *progname, char *filename);

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: executable filename numtimes\n");
    return 1;
  }
  int iterations = atoi(argv[2]);
  double time_taken = 0;

  for (int i = 0; i < iterations; i++) {
    long result = buffer_read_csv(argv[0], argv[1]);
    if (result < 0) {
      return 1;
    }
    time_taken += result;
  }

  time_taken /= iterations;

  printf("Mean Time Taken: %f\n", time_taken);

  return 0;
}

long buffer_read_csv(char *progname, char *filename) {
  long time_taken = 0;
  Matrix *csv_mat = NULL;
  int total_filename_str_size = strlen("/tmp/") + strlen(filename);
  char dump_filename[total_filename_str_size + 1];

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s: can't open %s\n", progname, filename);
    return -2;
  }

  long start = clock();

  char sml_buffer[SMALL_BUFFER_SIZE];
  size_t bytes_read;

  size_t file_idx = 0;
  int got_new_line = 0;
  int row = 0, col = 0;
  while ((bytes_read = fread(sml_buffer, 1, SMALL_BUFFER_SIZE, fp)) > 0) {
    // For calculating number of ',' i didn't use below method of using
    // bit manipulation cause first i need to find '\n'. One way is to look
    // up first and then doing the below thing. But rather than this, let's
    // just go by loop and read the ','.
    // Another way is to do both finding '\n' and calculating ',' at once
    // by following below method. But the instructions will increase and there
    // won't be any advantage of doing this.
    // Thus, i will just iterate through the buffer and count one by one
    for (size_t i = 0; i < bytes_read; i++) {
      if (sml_buffer[i] == '\n') {
        file_idx += i;
        got_new_line = 1;
        // for last column
        col++;
        break;
      }
      col += (sml_buffer[i] == ',');
    }
    if (got_new_line == 1) {
      break;
    }
    file_idx += bytes_read;
  }
  if (ferror(fp)) {
    fprintf(stderr, "Error reading the file\n");
    time_taken = -8;
    goto cleanup;
  }
  // to move to next char after '\n'
  file_idx++;

  if (fseek(fp, file_idx, SEEK_SET)) {
    fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
    fclose(fp);
    return -7;
  }

  int check = 0;
  char buffer[BUFFER_SIZE];
  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    /*
    Before looking down, let's look at ways we can have consider
    multiple char at once as that is my goal. One thing we can do is

    uli *s = (void *) string_ptr;

    Since because the way we define type for pointer which uses it's type
    to know how much bytes to read.

    If it is char * => then it will read one byte at a time if we dereference it
    Now, rather than reading one byte -> let's read 8 byte at once, thus that's
    why in above we typecasted it to void * to again recast to uli *. Since now
    the pointer is uli *, dereferencing it will result in reading 8 bytes at
    once. s still points to first byte but recasting makes it read 7 more byte
    which is ahead of first byte.

    So if my string is, "Hello world"

    H <- *s, let s store address 1 where H is stored.
    Now recasting s to uli * will result in reading 8 bytes which is

      H  e l l o' 'w o r l d
    0x48656c6c6f20776f726c64

    dereferencing of s will result in 0x48656c6c6f20776f rather than 0x48 which
    is one byte

    Note that, seeing integer form of above won't really make sense as it is
    converting above hex into integer. Let's look at as hex as that is how we
    are reading 8 byte of char at once.

    Another thing to note is that, printing the hex form of above won't result
    in like 0x48656c6c6f20776f but in 0x6f77206f6c6c6548 which is in reverse
    order. This is because my machine is little endian, thus it stores the most
    significant byte (which is 48) at lowest position.

    The most significant byte of 0x12345678 is 12, and 78 is the least
    significant byte
    In Big Endian, 0x12345678 is stored as 12 34 56 78 (most
    significant byte first)
    In Little Endian, 0x12345678 is stored as 78 56 34
    12 (least significant byte first)

    This is just how bytes are ordered and i don't think it will affect that
    much of our method

    Now coming back below, why did we shifted the address below to multiple of 8
    bytes which is sizeof(uli). The reason being, rather than having
    inconsistent pair where at end there can be any number of bytes left to
    look. Why not introduce system where we look each 8 byte at a time. We will
    resolve shifting address down by using _shift_bytes below. Thus effectively
    not looking at those bytes which doesn't belong to buffer.

    This way we read 8 bytes at once without variability shit handling at the
    end of buffer.
    */
    char *string = &buffer[0];
    uli *word_ptr = PTR_ALIGN_DOWN((void *)string, sizeof(uli));
    // converting address into integer for operation
    uintptr_t s = (uintptr_t)string;

    // last char byte
    // took care of uli overflow for address, if address integer is greater
    // than UINTPTR_MAX which is generally won't happen as UINTPTR_MAX is very
    // big. Still copied it from memchr.c as idk there can be possiblity.
    uintptr_t last_byte = ((s + bytes_read - 1) > UINTPTR_MAX)
                              ? UINTPTR_MAX
                              : (s + bytes_read - 1);
    uli *last_word_ptr = PTR_ALIGN_DOWN((void *)last_byte, sizeof(uli));
    // got last_word_ptr to use in loop as to only check for word which are
    // in middle and handling last word differently as like first word, this
    // word may contain memory not part of buffer.

    uli word = *word_ptr;

    uli res = find_similar_char(word, '\n'); // '\n' is 0x0a in hex
    // With this, we have essentially removed all the first bits
    // which weren't in buffer
    res = shift_bits(res, s);

    // for popcountll
    // lastly using gcc function to count number of 1
    // note that here we are using ll which is long long than popcountl
    // cause gcc considers long as 32 bit whereas above mine is 64 bits
    // Thus to avoid it we just play safe and use popcountll
    row += __builtin_popcountll(res);

    if (word_ptr == last_word_ptr) {
      // thus goes to end line of parent loop
      continue;
    }

    word = *++word_ptr;
    while (word_ptr != last_word_ptr) {
      row += __builtin_popcountll(find_similar_char(word, '\n'));
      word = *++word_ptr;
    }

    // now handling last word
    res = find_similar_char(word, '\n');
    if (__BYTE_ORDER == __LITTLE_ENDIAN) {
      res = res << (CHAR_BIT * (sizeof(uli) - (last_byte % sizeof(uli)) - 1));
    } else {
      res = res >> (CHAR_BIT * (sizeof(uli) - (last_byte % sizeof(uli)) - 1));
    }
    row += __builtin_popcountll(res);

    check = buffer[bytes_read - 1];
  }
  if (ferror(fp)) {
    fprintf(stderr, "Error reading the file\n");
    time_taken = -8;
    goto cleanup;
  }
  row += (check != '\n');

  if (fseek(fp, file_idx, SEEK_SET)) {
    fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
    fclose(fp);
    return -7;
  }

  size_t char_start = 0;

  csv_mat = Matrix_create(row, col);
  int read_columns = 0;
  int csv_indx = -1;
  double num = 0;

  while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
    char_start = 0;
    for (size_t i = 0; i < bytes_read; i++) {
      if ((buffer[i] == ',') || (buffer[i] == '\n')) {
        char temp = buffer[i];
        buffer[i] = '\0';
        // for "" case it gives 0.0
        // "" case when ,, or ,\n thus missing value
        // case. Below handling this when not
        // encountered number. Saving those missing values
        // as NAN
        num = atof(&buffer[char_start]);

        if (num == 0.0) {
          // let's check for "" or "\r" as these two are
          // possible for missing value
          // '\r' cause i got this when i tried editing
          // csv with neovim
          if ((char_start == i) ||
              ((buffer[char_start] == '\r') && (i == (char_start + 1)))) {
            num = NAN;
          } else {
            // now we need the format of
            // (0 to n zeros)(either a dot or not)(if
            // dot then 0 to m zeros)
            // any other string then it is invalid

            VALIDATE_ZERO_STRING(char_start, i, temp);
          }
        }

        buffer[i] = temp;
        csv_indx++;
        csv_mat->data[csv_indx] = num;
        read_columns++;

        char_start = i + 1;

        if (buffer[i] == '\n') {
          if (read_columns != col) {
            fprintf(stderr, "Incorrect file format\n");
            time_taken = -3;
            goto cleanup;
          }
          read_columns = 0;
        }
      }
    }

    if (bytes_read == BUFFER_SIZE) {
      if ((buffer[bytes_read - 1] != '\n') || (buffer[bytes_read - 1] != ',')) {
        file_idx += char_start;
        if (fseek(fp, file_idx, SEEK_SET)) {
          fprintf(stderr, "Can't Seek at %zu from start\n", file_idx);
          time_taken = -7;
          goto cleanup;
        }
      } else {
        file_idx += bytes_read;
      }
    } else if (buffer[bytes_read - 1] != '\n') {
      // now only possiblity is either a number
      // or any other unexpected char
      buffer[bytes_read] = '\0';
      num = atof(&buffer[char_start]);

      if (num == 0.0) {
        if ((char_start == (bytes_read - 1) && buffer[char_start] == '\r') ||
            (char_start == bytes_read)) {
          num = NAN;
        } else {
          VALIDATE_ZERO_STRING(char_start, bytes_read, '\0');
        }
      }

      csv_indx++;
      csv_mat->data[csv_indx] = num;
      read_columns++;

      if (read_columns != col) {
        fprintf(stderr, "Inside Ending with non \\n\n");
        fprintf(stderr, "Incorrect file format\n");
        time_taken = -4;
        goto cleanup;
      }
    }
  }

  long end = clock();
  time_taken = (end - start);

  matrix_print(csv_mat);
  /*
  dump_filename[total_filename_str_size] = '\0';
  dump_filename[0] = '/';
  dump_filename[1] = 't';
  dump_filename[2] = 'm';
  dump_filename[3] = 'p';
  dump_filename[4] = '/';
  for (int i = 5; i < total_filename_str_size; i++) {
    dump_filename[i] = filename[i];
  }
  matrix_dump_csv(csv_mat, dump_filename);
  */

cleanup:
  matrix_free(csv_mat);
  fclose(fp);

  return time_taken;
}
