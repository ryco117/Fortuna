Fortuna Generator
=================

This is my implementation of a Fortuna CSPRNG as described in *Practical Cryptography* by Niels Ferguson and Bruce Schneier.
256 bit AES is used as the block cipher (using the same AES class as my CryptoChat project and AES-NI from my AES-Asm project)
and scrypt (implemented using libscrypt) is used as the hash function. Seeding the generator once (recommended to use 256 bits of entropy) is
required before the generator can be used. The resulting state can generate n pseudo random 16 byte blocks (where n is less than 65536 blocks) at a time,
2^128 times without a cycle. Included with the class is main.cpp and base64.h for example usage of the class, and not required, however,
it is very helpful for evaluating the prng. The example can be generated with `make` for *nix systems and for windows with `make win`
(note this only controls where the example grabs the seed from and how the AES-NI assembly routine expects the parameters to be passed, the rest is completely portable).

The example output file can be tested for statistical randomness using the program ENT found at http://www.fourmilab.ch/random/
and using dieharder http://www.phy.duke.edu/~rgb/General/dieharder.php

I have included an example output of dieharder reading raw output from the Fortuna Generator from stdin.
Here, all the tests passed except one, which reported weak (however, it is important to note that truly random sets of
values will fail these tests 1 in 100 times on average, and there are 116 tests, leading to a probability of 31.2% chance that all tests pass
given truly random data).
