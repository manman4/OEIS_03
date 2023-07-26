seq(n) = my(A=1); for(i=1, n, A=1 + x*A^3 + x^2*A +x*O(x^n)); Vec(A);  
seq(35)  
