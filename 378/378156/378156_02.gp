\\ G.f. A(x) satisfies A(x) = ( 1 + x * A(x)^(1/2) * (1 + A(x)^(1/2)) )^4.
seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A^(1/2) * (1 + A^(1/2)) )^4 + x*O(x^n) ); Vec(A);
seq(25)    

\\ A^(1/4) = A219534
seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A^(1/2) * (1 + A^(1/2)) )^4 + x*O(x^n) ); Vec(A^(1/4));
seq(25)    

