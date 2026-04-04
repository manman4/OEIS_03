\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)^2 + x^2*A(x)*d/dx(A(x)^2).
my(A=1, N=30); for(k=1, N, A=1+x*A^2+x^2*A*deriv(A^2)+x*O(x^N)); Vec(A)
  