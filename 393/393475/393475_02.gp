\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)*(1 + E^2(A(x))) + x*E^3(A(x)).
my(A=1, N=30); for(k=1, N, A=1 + x*A*(1 + x*deriv(x*deriv(A))) + x*x*deriv(x*deriv(x*deriv(A))) + x*O(x^N)); Vec(A)

E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1 + x*A*(1 + E(E(A))) + x*E(E(E(A))) + x*O(x^N)); Vec(A)