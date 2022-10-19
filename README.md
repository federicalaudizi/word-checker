# Word-Checker

### Summary

The aim of the project is to create a system which checks the corrispondence between two strings of equal lenght.
The strings can contain uppercase or lowercase letters, numbers and symbols such as '-' and '_'.
The system reads from standard input a sequence of instructions and data, and prints strings in output as appropriate.

### Detailed description

The system reads from stdin:
- a k value, which indicates the length of the words.
- a sequence (of arbitrary length) of words, each of length k, which constitutes the set of admissible words.
    - it is also assumed that the sequence of words does not contain duplicates.

At this point, a sequence of "games" is read from standard input in which the beginning of each new game is marked by the command `+new_game`.
<br />The sequences of input strings (following the command `+new_game`) are made as follows: <br />
- reference word (length of k characters).<br />
- maximum n number of words to compare with the reference word. <br />
- sequence of words (each of k characters) to be compared with the reference word. <br />

For each word read 'p', to be compared with the reference word 'r', the program writes a sequence of
k characters to stdout as follows:

- From now on, we are giong to indicate with p[1], p[2], ... p[k] the characters of the word p, with r[1], r[2], ... r[k] those of the word r, and with res[1], res[2], ... res[k] those of the printed sequence.

For every 1 ≤ i ≤ k, we have that:
- res[i] is the '+' character if the i-th character of p is equal to the i-th character of r. <br />
    - p[i] is "in the correct position".
- res [i] is the '/' character if p [i] does not appear anywhere in r.
- res [i] is the '|' character if p [i] appears in r, but not in the i-th position; however,
if n instances of p [i] appear in r, if c<sub>i</sub> is the number of instances of the character
p [i] that are in the correct position (clearly c<sub>i</sub> ≤ n<sub>i</sub>) and if there are before the i-th
character in p at least n<sub>i</sub>-c<sub>i</sub> characters equal to p [i] which are in the wrong position,
then res [i] must be / instead of |.

If a word that does not belong to the set of admissible ones is read from standard input,
the program writes the string 'not_exists' to standard output (NB: it is not counted as an attempt).<br />
If instead the word r is read (ie if p = r), then the program writes 'ok' (without printing the detailed result of the comparison),and the game ends.<br />
If, after reading n admissible words (with n maximum number of words to compare with r),
none of these were equal to r,
the program writes 'ko' (after printing the result of the comparison of the last word), and the game ends.<br />

After the game is over:
- There may be no other words to compare (but there may be
the insertion of new admittable words).
- If the user gives in input the command `+new_game`, a new game starts (the admissble words remain inaltered).

Any comparison between p and r produces constraints learned from the comparison which will be explained below.


## Commands
### `+print_filtered`
Occasionally, in the sequence of input strings, the command `+print_filtered` may appear, the program must produce in output, in lexicographic order, the set of permissible words that are compatible with the constraints learned up to that point in the game, written one by line.
<br />Note that the constraints concern, for each symbol:

### Constraints


 1. if the symbol does not belong to r.

 2. places where that symbol must appear in r.

 3. places where that symbol cannot appear in r.

 4. minimum number of times the symbol appears in r.

 5. exact number of times the symbol appears in r.

In addition, after each comparison, the program must print in output the number of eligible words still compatible with the constraints learned

### `+insert_init` & `+insert_end`
Both during a game and between one game and another, the commands `+insert_init` and `+insert_end` may appear,
enclosing a sequence of new words to add to the set of admissible words (only if compatible with the constraints learned up to that point)
- the added words are also of length k, and it is always assumed that there are no
duplicate words (not even with respect to the words already present in the set of admissibles).
<br />

## Implementation

### Set up & comparisons
This project was implemented using mainly Hash Tables (in which collisions where handled with the chaining technique) which where chosen for thir efficency in lookups.
All the admissible words are saved in a Table, before proceding with the comparison it is verified whether the word can
actually be compared (O(1)) or 'not_exists' should be printed. Subsequently, if the check is successful, it starts filling the string
'res': initially iterating to fill it up with the '+' and then for the rest, therefore it loops twice k times, so it takes a time complexity equal to Θ(k).

### Constraints
Constraints are verified comparing each word from the Table of the admissible words with the string 'out',
the way how each constraint has been saved for when `+insert_init` is given in input was implemented as follows:
 1. Constraint 1 & 3: Array[k] of Hash Tables, if a character appears in the k-th Hash table of the element it must not apper in the k-th char of the word.
 2. Constraint 2 & 5: Array[k], if a character appears in the k-th element of the array it must appear in the k-th char of the word.
 3. Constraint 4: Hash table having for key each charthat must apper in the word and as value the minimum number of times.

Verification of the saved constraints for each new word is done in O(k).

### Sorting
The sorting of the filtered words in lexicograph order was implemented with the __Quicksort__ algorithm for its efficency in time complexity O(nlogn) in the avarage case.
