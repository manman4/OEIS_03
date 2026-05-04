a386367(n) = sum(k=0, n-1, binomial(5*k, k)*binomial(5*n-5*k-2, n-k-1));
for(n=0, 25, print1(a386367(n), ", "));
\\ a(n) = (n+1) * A386367(n+1).
a(n) = (n+1) * a386367(n+1);
for(n=0, 25, print1(a(n), ", "));



