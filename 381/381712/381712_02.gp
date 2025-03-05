M=38;

J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a194532(n) = J(n, 6)/J(n, 2);
for(n=1, M, print1(a194532(n), ", "));

\\ G.f.: 1/Product(k>=1} (1 - x^k)^(k * A194532(k)).
my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^(k * a194532(k))))


