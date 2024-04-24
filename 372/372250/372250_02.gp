seq(n) = my(A=1); for(i=1, n, A=exp( x * A * (1 + x^2 * A +x*O(x^n)) ) ); Vec(serlaplace(A));
seq(35)    

