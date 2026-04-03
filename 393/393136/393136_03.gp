\\ G.f.: 1 - E(log(1 - x*B(x))), where B(x) is the g.f. of A393959.
E(f) = x*deriv(f);
my(A=1, N=20); for(k=1, N, A=1-E(E(log(1-x*A)))+x*O(x^N)); Vec(1 - E(log(1-x*A)))