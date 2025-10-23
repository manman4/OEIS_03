M=18;

a(n, q=1, r=0, s=3, t=-1, u=0) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+q*u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * (3*k+1)^(k-1) * binomial(n-1,n-k)/k!.
b(n) = n!*sum(k=0, n, (-1)^(n-k)*(3*k+1)^(k-1)*binomial(n-1, n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));


