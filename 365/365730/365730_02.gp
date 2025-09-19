seq(n) = my(A=1); for(i=1, n, A=1 + x^4*A^4*(1 + x*A +x*O(x^n)) ); Vec(A);           
seq(40) 


