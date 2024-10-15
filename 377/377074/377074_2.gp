my(A=1, n=22); for(i=1, n, A=exp( 3 * (exp(x*A^(1/3)) - 1) ) +x*O(x^n)  ); Vec(serlaplace(A))
