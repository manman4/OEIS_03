M=18;

a(n) = sum(k=0, n\2, (n-k)^n*binomial(n-k, k));
for(n=0, M, print1(a(n), ", "));