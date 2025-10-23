M=17;

a(n, q=1, r=0, s=2, t=3, u=0) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+q*u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (2*k+1)^(k-1) * binomial(3*k,n-k)/k!.
b(n) = n!*sum(k=0, n, (2*k+1)^(k-1)*binomial(3*k, n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));


