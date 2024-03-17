seq(n) = my(A=1); for(i=1, n, A=1 + x*A^4 * (exp(x*A^3 +x*O(x^n)) - 1) ); Vec(serlaplace(A));         
seq(20)   