\\ G.f.: exp(Sum_{k>=1} (-1)^(k-1) * A050352(k) * x^k/k).
a050352(n) = if(n==0, 1, sum(k=0, n, 3^(k-1) * k! * stirling(n, k, 2)));
for(n=0, 20, print1(a050352(n), ", "));
my(N=30, x='x+O('x^N)); Vec(exp(sum(k=1, N, (-1)^(k-1) * a050352(k) * x^k/k)))
