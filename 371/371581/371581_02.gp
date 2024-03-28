seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^(5/2) / (1-x*A) +x*O(x^n) )^2 ); Vec(A);
seq(25)

\\ A365192
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A^(5/2) / (1-x*A) +x*O(x^n) )^2 ); Vec(A^(1/2));
seq(25)

