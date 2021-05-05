M=10;

a(n) = sum(k=0, n, k!^n*binomial(n, k));
for(n=0, M, print1(a(n), ", "));