seq(n) = my(A=1); for(i=1, n, A=2 - exp(-x*A^3) +x*O(x^n)); Vec(serlaplace(A));         
seq(30)    


