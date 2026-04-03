\\ G.f. A(x) satisfies A(x) = 1 + x*A(x)*(1 + E^3(A(x))) + x*E^4(A(x)).
my(A=1, N=30); for(k=1, N, A=1 + x*A*(1 + x*deriv(x*deriv(x*deriv(A)))) + x*x*deriv(x*deriv(x*deriv(x*deriv(A)))) + O(x^N)); Vec(A)
