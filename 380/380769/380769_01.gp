M=17;

a(n, q=1, r=2, s=0, t=1, u=1) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial((r*u+1)*n+((s-r)*u+t-1)*k+q*u-1, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (2*n-2*k+1)^(k-1) * binomial(3*n-2*k,n-k)/k!.
b(n) = n!*sum(k=0, n, (2*n-2*k+1)^(k-1)*binomial(3*n-2*k,n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));


