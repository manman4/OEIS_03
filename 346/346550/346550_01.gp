M=21;

a(n) = sum(k=n\2, n, k!*binomial(k+1, n-k));
for(n=0, M, print1(a(n), ", "));