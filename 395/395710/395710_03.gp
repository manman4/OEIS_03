a386566(n) = sum(k=0, n-1, binomial(5*k-1, k)*binomial(5*n-5*k, n-k-1));
for(n=0, 25, print1(a386566(n), ", "));
\\ a(n) = (n+1) * A386566(n+1).
a(n) = (n+1) * a386566(n+1);
for(n=0, 25, print1(a(n), ", "));



