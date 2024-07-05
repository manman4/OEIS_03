b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
for(n=1, 40, print1(b(n, 3),", "));

my(N=30, x='x+O('x^N)); Vec(1/prod(k=1, N, (1 - x^k)^b(k, 3))^3)