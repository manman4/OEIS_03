M=17;

a(n, r=1, s=1, t=2, u=1) = n!*sum(k=0, n, (r*n+(s-r)*k+1)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (n+1)^(k-1) * binomial(n+2*k+1,n-k)/k!.
b(n) = n!*sum(k=0, n, (n+1)^(k-1)*binomial(n+2*k+1,n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));


