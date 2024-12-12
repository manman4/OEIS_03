\\ A(x) = 1 + x * A(x) * (1 + A(x)^(2/3) + A(x)^(4/3)).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A*(1 + A^(2/3) + A^(4/3)) + x*O(x^n) ); Vec(A);
seq(30)

\\ A(x) = B(x)^3 where B(x) is the g.f. of A271469.
seq(n) = my(A=1); for(i=1, n, A=1 + x*A*(1 + A^(2/3) + A^(4/3)) + x*O(x^n) ); Vec(A^(1/3));
seq(30)