seq(n) = my(A=1); for(i=1, n, A=1/(3 - 2*exp(x*A^2 +x*O(x^n))) ); Vec(serlaplace(A));         
seq(20)   