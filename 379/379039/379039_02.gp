M=25;

\\ G.f. A(x) satisfies A(x) = ( (1 + x) * (1 + x*A(x)^2) )^2.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x) * (1 + x*A^2) + x*O(x^n))^2); Vec(A);
seq(M)

\\ G.f.: A(x) = B(x)^2 where B(x) is the g.f. of A364337.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x) * (1 + x*A^2) + x*O(x^n))^2); Vec(A^(1/2));
seq(M)