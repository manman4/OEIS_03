seq(n) = my(A=1); for(i=1, n, A=1-x*A^3*(1 - 3*A +x*O(x^n))  ); Vec(A);         
seq(18)   

