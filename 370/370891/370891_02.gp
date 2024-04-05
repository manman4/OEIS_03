seq(n) = my(A=1); for(i=1, n, A=(1 + x * A^(1/4) * (1+A^(3/4)) +x*O(x^n) )^2 ); Vec(A);
seq(25)    

\\ A^(1/4) = A106228
seq(n) = my(A=1); for(i=1, n, A=(1 + x * A^(1/4) * (1+A^(3/4)) +x*O(x^n) )^2 ); Vec(A^(1/4));
seq(25)
