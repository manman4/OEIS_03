seq(n) = my(A=1); for(i=1, n, A=exp( x * A^2 * (1 + A +x*O(x^n))/2 ) ); Vec(serlaplace(A));
seq(35)    

