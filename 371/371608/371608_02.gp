seq(n) = my(A=1); for(i=1, n, A=(1 + x * (1+x*A^3) +x*O(x^n) )^2 ); Vec(A);
seq(25)

\\ A137966
seq(n) = my(A=1); for(i=1, n, A=(1 + x * (1+x*A^3) +x*O(x^n) )^2 ); Vec(A^(1/2));
seq(25)
