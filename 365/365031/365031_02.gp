seq(n) = my(A=1); for(i=1, n, A=exp(x*A*(1 + x*A +x*O(x^n))^2 ) ); Vec(serlaplace(A));         
seq(18)
