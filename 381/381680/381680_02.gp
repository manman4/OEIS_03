M=20;

j(n, k) = sumdiv(n, d, d^k*moebius(n/d)); 
a115224(n) = n^2 * j(n, 3);
for(n=1, M, print1(a115224(n), ", "));

\\ G.f.: 1/Product(k>=1} (1 - x^k)^A115224(k).
my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^a115224(k)))
