\\ Cf. A260332

seq(n) = my(A=1); for(i=1, n, A=1 + x * A^4 * (1+A) +x*O(x^n)); Vec(A^4); 
seq(19)
