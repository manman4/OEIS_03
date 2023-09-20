\\ Cf. A144097

seq(n) = my(A=1); for(i=1, n, A=1 + x * A^3 * (1+A) +x*O(x^n)); Vec(A^3); 
seq(19)
