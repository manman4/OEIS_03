\\ G.f.: exp(5 * Sum_{k>=1} (-1)^(k-1) * A094417(k) * x^k/k).
a094417(n) = sum(k=0, n, 4^k * k! * stirling(n, k, 2));
for(n=0, 20, print1(a094417(n), ", "));
my(N=30, x='x+O('x^N)); Vec(exp(5 * sum(k=1, N, (-1)^(k-1) * a094417(k) * x^k/k)))
