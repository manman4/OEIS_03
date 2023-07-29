seq(n) = my(A=1); for(i=1, n, A=1/(1 - x +x*O(x^n)) + x^2 * (1-x) * A^3 ); Vec(A); 
seq(20)                                                                             
