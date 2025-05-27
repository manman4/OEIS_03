\\ G.f.: exp(3 * Sum_{k>=1} (-1)^(k-1) * A004123(k+1) * x^k/k).
a(n) = sum(k=0, n, 2^k * k! * stirling(n, k, 2));
a004123(n) = a(n-1);
for(n=1, 20, print1(a004123(n), ", "));
my(N=30, x='x+O('x^N)); Vec(exp(3 * sum(k=1, N, (-1)^(k-1) * a004123(k+1) * x^k/k)))
