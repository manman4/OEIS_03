seq(n) = my(A=1); for(i=1, n, A=1 + x * (1 + 1/A^2) +x*O(x^n)); Vec(A);      
seq(35)    

seq(n) = my(A=1); for(i=1, n, A=1 + x * (1 + 1/A^2) +x*O(x^n)); Vec(1/A);
seq(35)      