\\ G.f. A(x) satisfies A(x) = ( 1 + x * A(x)^(2/3) * (1 + A(x)^(2/3)) )^3.
seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A^(2/3) * (1 + A^(2/3)) )^3 + x*O(x^n) ); Vec(A);
seq(25)    

\\ A^(1/3) = A219534
seq(n) = my(A=1); for(i=1, n, A=( 1 + x * A^(2/3) * (1 + A^(2/3)) )^3 + x*O(x^n) ); Vec(A^(1/3));
seq(25)    

