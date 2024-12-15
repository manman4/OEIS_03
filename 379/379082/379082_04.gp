M=25;

\\ A(x) = ( (1 + x*A(x)) * (1 + x^3*A(x)^(7/2)) )^2.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x^3*A^(7/2)) + x*O(x^n))^2); Vec(A);
seq(M)

\\ A(x) = B(x)^2 where B(x) is the g.f. of A379089.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x^3*A^(7/2)) + x*O(x^n))^2); Vec(A^(1/2));
seq(M)