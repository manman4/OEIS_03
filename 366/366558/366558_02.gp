seq(n) = my(A=1); for(i=1, n, A=1 + x + x^4*A^4 +x*O(x^n) ); Vec(A); 
seq(30)
         