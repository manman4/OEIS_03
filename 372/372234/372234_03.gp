seq(n) = my(A=1); for(i=1, n, A=exp( 2 * x * (1 + x^2 * A^(1/2) +x*O(x^n)) ) ); Vec(serlaplace(A));
seq(35)    

