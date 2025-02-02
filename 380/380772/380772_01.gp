M=17;

a(n, r=2, s=0, t=1, u=1) = n!*sum(k=0, n, (r*n+(s-r)*k+1)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (2*n-2*k+1)^(k-1) * binomial(2*n-k+1,n-k)/k!.
b(n) = n!*sum(k=0, n, (2*n-2*k+1)^(k-1)*binomial(2*n-k+1,n-k)/k!);  
for(n=0, 100, print1(a(n)-b(n),", "));


