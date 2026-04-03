\\ G.f. A(x) satisfies A(x) = 1 - E^2(log(1 - x*A(x))), where E is the Euler operator x*d/dx.
my(A=1, N=30); for(k=1, N, A=1 - x*deriv(x*deriv(log(1 - x*A))) + x*O(x^N)); Vec(A)

E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1-E(E(log(1-x*A)))+x*O(x^N)); Vec(A)
