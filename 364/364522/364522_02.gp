seq(n) = my(A=1); for(i=1, n, A=1 + x*A + x^5*A^5 +x*O(x^n)); Vec(A);  
seq(35)  
