M=25;

\\ G.f. A(x) satisfies A(x) = ( (1 + x*A(x)^2) * (1 + x*A(x)) )^2.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A^2) * (1 + x*A) )^2 + x*O(x^n)); Vec(A);
seq(M)

\\ G.f.: B(x)^2 where B(x) is the g.f. of A215715.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A^2) * (1 + x*A) )^2 + x*O(x^n)); Vec(A^(1/2));
seq(M)