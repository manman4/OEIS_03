seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^2/(1-x) +x*O(x^n) )^3 ); Vec(A);
seq(25)

\\ A349333
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^2/(1-x) +x*O(x^n) )^3 ); Vec(A^(1/3));
seq(25)
