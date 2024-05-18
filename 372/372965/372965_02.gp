a(n) = sumdiv(n, d, eulerphi(d^5));
for(n=1, 33, print1(a(n), ", "))

my(N=80, x='x+O('x^N)); Vec(sum(k=1, N, k^4 * eulerphi(k) * x^k/(1 - x^k) ))