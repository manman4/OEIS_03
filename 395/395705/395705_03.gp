a308523(n) = sum(k=0, n-1, binomial(4*k-2, k)*binomial(4*n-4*k, n-k-1));
for(n=0, 25, print1(a308523(n), ", "));
\\ a(n) = (n+1) * A308523(n+1).
a(n) = (n+1) * a308523(n+1);
for(n=0, 25, print1(a(n), ", "));



