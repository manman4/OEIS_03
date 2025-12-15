seq(n) = my(A=1); for(i=1, n, A=1 + x*A /(1 - x*A^2 +x*O(x^n))^3 ); Vec(A);                              
seq(30)            
