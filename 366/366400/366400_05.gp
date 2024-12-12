\\ A(x) = 1 + x * A(x) * (1 + A(x)^(3/2)).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A*(1 + A^(3/2)) + x*O(x^n) ); Vec(A);
seq(30)

\\ A(x) = B(x)^2 where B(x) is the g.f. of A219537.
seq(n) = my(A=1); for(i=1, n, A=1 + x*A*(1 + A^(3/2)) + x*O(x^n) ); Vec(A^(1/2));
seq(30)