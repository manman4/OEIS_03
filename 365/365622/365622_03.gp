\\ Cf. A363006

seq(n) = my(A=1); for(i=1, n, A=1 + x * A^5 * (1+A) +x*O(x^n)); Vec(A^5); 
seq(19)
