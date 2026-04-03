\\ G.f. A(x) satisfies A(x) = 1 - E(log(1 - x*(1 + E^3(A(x))))), where E is the Euler operator x*d/dx.
my(A=1, N=30); for(k=1, N, A=1 - x*deriv(log(1 - x*(1 + x*deriv(x*deriv(x*deriv(A)))))) + x*O(x^N)); Vec(A)
