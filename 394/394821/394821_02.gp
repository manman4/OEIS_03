\\ G.f.: 1 + E^3(B(x)), where B(x) is the g.f. of A393775.
E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1+x*A*(1+E(E(E(A))))+x*E(E(E(E(A))))+x*O(x^N)); Vec(1 + E(E(E(A))))