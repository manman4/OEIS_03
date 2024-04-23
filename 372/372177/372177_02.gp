seq(n) = my(A=1); for(i=1, n, A=exp( x * A^(1/2) * (1 + A^(1/2) +x*O(x^n)) ) ); Vec(serlaplace(A));
seq(35)    

\\ A138860
seq(n) = my(A=1); for(i=1, n, A=exp( x * A^(1/2) * (1 + A^(1/2) +x*O(x^n)) ) ); Vec(serlaplace(A^(1/2)));
seq(35)    
