# Hashtable

Hashtables can perform as bad as `O(n)` just like a linked list, but more often
than not it operates at `O(1)` **WHEN** the hash-table slots is at least 
proportional to the number of elements in the table and  doubly linked list are 
used for the cases of chaining. 

The idea behind hashtables is to store values in some array. Instead of using
the index of the value stored in the array to look up the value we instead use
the `keys` of the value to determine the index of where the value is stored
in the array.

The index of the array is calculated by performing some kind of hashing of the
key. The output of the hash will determine the index of where the value is 
stored. This does introduce the possibility of colliding with other indexes. 

There are two common ways of resolving the collision, `chaining` and `open addressing`.

## Chaining 

Chaining is when you store the address of the linked list into the `satellite data`.
The `satellite data` is normally where the value for the key will be stored, 
but with chaining you instead store a pointer to the link list. The link list
will then contain a list of the values. Searching it still faster than iterating
over a link list in the case where you have a lot of items because you 
significantly reduce the amount of items to inspect. 

It is important to note that if you intend to support deletion it is crucial 
to use a doubly linked list to be able to reduce the deletion penalty as much
as possible. 