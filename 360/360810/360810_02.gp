M=28;

a(n) = sum(k=0, n\2, (n-2*k)^k*binomial(n-k-1, k));
for(n=0, M, print1(a(n), ", "));

