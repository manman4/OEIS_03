M=33;

J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a065960(n) = J(n, 8)/J(n, 4);
for(n=1, M, print1(a065960(n), ", "));

\\ G.f.: 1/Product(k>=1} (1 - x^k)^(k^3 * A065960(k)).
my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^(k^3 * a065960(k))))



