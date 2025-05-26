\\ G.f.: exp(3 * Sum_{k>=1} A004123(k) * x^k/k).
a004123(n) = sum(k=0, n, 2^k * k! * stirling(n, k, 2));
my(N=20, x='x+O('x^N)); Vec(exp(3 * sum(k=1, N, a004123(k) * x^k/k)))
