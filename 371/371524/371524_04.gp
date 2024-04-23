seq(n) = my(A=1); for(i=1, n, A=exp( 2 * x * (1 + A^(1/4) +x*O(x^n)) ) ); Vec(serlaplace(A));
seq(35)    

