M=18;

a(n) = sum(k=0, n\2, (n-2*k)^n*binomial(n-k-1, k));
for(n=0, M, print1(a(n), ", "));

