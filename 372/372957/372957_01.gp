b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
my(N=40, x='x+O('x^N)); Vec(x/prod(k=1, N, (1 - x^k)^b(k, 2))^2)