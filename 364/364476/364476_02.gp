seq(n) = my(A=1); for(i=1, n, A=1 + x*A + x^2*A^7 +x*O(x^n)); Vec(A);  
seq(35)  
