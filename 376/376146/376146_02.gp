\\ 	E.g.f. satisfies A(x) = exp( x * (1+x)^4 * A(x) ).
seq(n) = my(A=1); for(i=1, n, A=exp(x*(1+x)^4*A + x*O(x^n))); Vec(serlaplace(A));
seq(20)