a386567(n) = sum(k=0, n-1, binomial(6*k-1, k)*binomial(6*n-6*k, n-k-1));
for(n=0, 25, print1(a386567(n), ", "));
\\ a(n) = (n+1) * A386567(n+1).
a(n) = (n+1) * a386567(n+1);
for(n=0, 25, print1(a(n), ", "));



