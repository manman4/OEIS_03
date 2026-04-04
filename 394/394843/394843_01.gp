\\ G.f. A(x) satisfies A(x) = 1 / (1 - x/2 * d/dx log(1 + 2*x*A(x))).
my(A=1, N=30); for(k=1, N, A=1/(1-x/2*deriv(log(1+2*x*A))+x*O(x^N))); Vec(A)
  