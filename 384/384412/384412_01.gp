\\ G.f.: exp((1/10) * Sum_{k>=1} b(2*k) * x^k/k), where b(n) = Sum_{k=0..n} 2^k * k! * Stirling2(n,k).
b(n) = sum(k=0, n, 2^k*k!*stirling(n, k, 2));
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, b(2*k)*x^k/k)/10))

print("n>=1のときのb(2*n)/10")
for(n=1, 30, print(n, " ", b(2*n)/10));