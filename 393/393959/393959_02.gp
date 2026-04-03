\\ G.f.: 1 + E(B(x)), where B(x) is the g.f. of A393136.
E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1+x*A*(1+E(A))+x*E(E(A))+x*O(x^N)); Vec(1 + E(A))