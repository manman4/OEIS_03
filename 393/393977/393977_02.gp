\\ G.f.: 1 + E^2(B(x)), where B(x) is the g.f. of A393475.
E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1+x*A*(1+E(E(A)))+x*E(E(E(A)))+x*O(x^N)); Vec(1 + E(E(A)))