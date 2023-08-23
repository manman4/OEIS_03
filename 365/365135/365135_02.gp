seq(n) = my(A=1); for(i=1, n, A=(1 + x*A/(1 - x*A +x*O(x^n))^3 )^2 ); Vec(A);         
seq(18)
