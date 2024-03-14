seq(n) = my(A=1); for(i=1, n, A=exp(x*A^2 * (exp(x*A^2 +x*O(x^n))-1)) ); Vec(serlaplace(A));  
seq(20)   