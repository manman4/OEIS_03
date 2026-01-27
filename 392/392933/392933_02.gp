M=30;

\\ E.g.f.: (1/x) * Series_Reversion( x/(1 - x*log(1-x^2)) ).
seq(n) = my(A=1); for(i=1, n, A=1 - x*A*log(1 - x^2*A^2) + x*O(x^n)); Vec(serlaplace(A));
seq(M) 