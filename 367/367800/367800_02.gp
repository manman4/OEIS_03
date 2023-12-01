seq(n) = my(A=1); for(i=1, n, A=1/(1 - x * subst(A, x, x^8) +x*O(x^n))); Vec(A); 
seq(50)        