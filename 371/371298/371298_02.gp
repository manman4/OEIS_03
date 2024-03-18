seq(n) = my(A=1); for(i=1, n, A=2/(3 - exp(2*x*A^2 +x*O(x^n))) ); Vec(serlaplace(A));         
seq(20)   