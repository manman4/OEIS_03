seq(n) = my(A=1); for(i=1, n, A=1 + x /(1 - x*A^3 +x*O(x^n))^2 ); Vec(A);                              
seq(30)            