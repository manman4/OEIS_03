seq(n) = my(A=1); for(i=1, n, A=1 - log(1-x*A^3) +x*O(x^n)); Vec(serlaplace(A));         
seq(30)    


