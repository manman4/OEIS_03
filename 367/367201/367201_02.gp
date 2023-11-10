seq(n) = my(A=1); for(i=1, n, A=exp( (exp(x*A +x*O(x^n)) - 1) * A^3) ); Vec(serlaplace(A));         
seq(30)    


