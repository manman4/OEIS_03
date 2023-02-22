M=32;

a(n) = sum(k=0, n\3, (n-3*k)^k*binomial(n-2*k-1, k));
for(n=0, M, print1(a(n), ", "));

