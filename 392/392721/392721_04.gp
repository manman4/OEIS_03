M=30;

\\ E.g.f.: -log(1 - B(x)), where B(x) is the e.g.f. of A392722.
seq(n) = my(A=x); for(i=1, n, A=x + log(1 - A)^4 + x*O(x^n)); Vec(serlaplace(-log(1 - A)));
seq(M)