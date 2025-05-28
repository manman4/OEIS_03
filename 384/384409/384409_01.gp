\\ G.f.: exp((1/3) * Sum_{k>=1} A000670(4*k) * x^k/k).
a000670(n) = sum(k=0, n, k!*stirling(n, k, 2));
for(n=0, 20, print1(a000670(n), ", "));
my(N=20, x='x+O('x^N)); Vec(exp((1/3) * sum(k=1, N, a000670(4*k)*x^k/k)))

print("n>=1のときのa000670(4*n)/3")
for(n=1, 30, print(n, " ", a000670(4*n)/3));