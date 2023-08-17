seq(n) = my(A=1); for(i=1, n, A=exp(x*A^3*(1 + x*A^3 +x*O(x^n)) ) ); Vec(serlaplace(A));         
seq(18)                                                                                      


