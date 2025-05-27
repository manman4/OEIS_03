\\ G.f.: exp(4 * Sum_{k>=1} (-1)^(k-1) * A032033(k) * x^k/k).
a032033(n) = sum(k=0, n, 3^k * k! * stirling(n, k, 2));
for(n=0, 20, print1(a032033(n), ", "));
my(N=30, x='x+O('x^N)); Vec(exp(4 * sum(k=1, N, (-1)^(k-1) * a032033(k) * x^k/k)))
