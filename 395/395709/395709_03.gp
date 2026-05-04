a386565(n) = sum(k=0, n-1, binomial(4*k-1, k)*binomial(4*n-4*k, n-k-1));
for(n=0, 25, print1(a386565(n), ", "));
\\ a(n) = (n+1) * A386565(n+1).
a(n) = (n+1) * a386565(n+1);
for(n=0, 25, print1(a(n), ", "));



