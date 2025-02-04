M=16;

a(n, q=2, r=1, s=1, t=2, u=3) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+q*u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = 2 * n! * Sum_{k=0..n} (n+2)^(k-1) * binomial(3*n+2*k+6,n-k)/k!.
b(n) = 2 * n! * sum(k=0, n, (n+2)^(k-1)*binomial(3*n+2*k+6,n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));


