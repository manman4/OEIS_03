seq(n) = my(A=1); for(i=1, n, A=1/(1 - x*A^2 +x*O(x^n))^(x*A^2) ); Vec(serlaplace(A));  
seq(20)   