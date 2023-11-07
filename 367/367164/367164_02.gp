seq(n) = my(A=1); for(i=1, n, A=1 + A^3 * (1 - exp(-x +x*O(x^n)) )); Vec(serlaplace(A));         
seq(30)    


