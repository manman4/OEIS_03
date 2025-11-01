\\ a(n) = n! * Sum_{k=0..n} (2*k+1)^(k-1) * binomial(n+2*k-1,n-k)/k!.
b(n) = n! * sum(k=0, n, (2*k+1)^(k-1) * binomial(n+2*k-1,n-k)/k!);
for(n=0, 17, print1(b(n),", "));   

a(n, q=1, r=0, s=2, t=3, u=0) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial((r*u+1)*n+((s-r)*u+t-1)*k+q*u-1, n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));   


