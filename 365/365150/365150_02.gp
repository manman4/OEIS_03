seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^2/(1 - x*A +x*O(x^n))^3 )^1 ); Vec(A);         
seq(18)
