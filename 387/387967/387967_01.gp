a(n, q=1, r=1, s=1, t=3, u=1) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+q*u, n-k)/k!);
for(n=0, 17, print1(a(n),", "));

\\ b(n) = n! * Sum_{k=0..n} (n+1)^(k-1) * binomial(n+3*k+1,n-k)/k!.
b(n) = n! * sum(k=0, n, (n+1)^(k-1) * binomial(n+3*k+1,n-k)/k!);
for(n=0, 50, print1(a(n)-b(n),", "));   


