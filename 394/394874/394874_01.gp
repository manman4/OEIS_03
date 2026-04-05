M=30;

\\ G.f. A(x) satisfies A(x) = 1 - x - x * d/dx log(1 - x*A(x)).
my(A=1, N=M); for(k=1, N, A=1-x-x*deriv(log(1-x*A))+x*O(x^N)); Vec(A) 