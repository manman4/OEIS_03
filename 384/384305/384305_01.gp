\\ G.f.: exp(6 * Sum_{k>=1} A094418(k) * x^k/k).
a094418(n) = sum(k=0, n, 5^k * k! * stirling(n, k, 2));
for(n=0, 20, print1(a094418(n),", "));

my(N=20, x='x+O('x^N)); Vec(exp(6 * sum(k=1, N, a094418(k) * x^k/k)))
