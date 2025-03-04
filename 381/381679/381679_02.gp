M=20;

j(n, k) = sumdiv(n, d, d^k*moebius(n/d)); 
a000056(n) = n * j(n, 2);
for(n=1, M, print1(a000056(n), ", "));

\\ G.f.: 1/Product(k>=1} (1 - x^k)^A000056(k).
my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^a000056(k)))


