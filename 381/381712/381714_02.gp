M=28;

J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a381713(n) = J(n, 9)/J(n, 3);
for(n=1, M, print1(a381713(n), ", "));

\\ G.f.: 1/Product(k>=1} (1 - x^k)^(k^2 * A381713(k)).
my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^(k^2 * a381713(k))))

