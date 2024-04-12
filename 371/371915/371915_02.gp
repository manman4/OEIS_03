seq(n) = my(A=1); for(i=1, n, A=1 - x/A^2 * (1 - A - A^5) +x*O(x^n)); Vec(A);      
seq(35)    