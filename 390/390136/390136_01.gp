M=21;

a(n, q=1, r=1, s=1, t=-1, u=0) = q*n!*sum(k=0, n, (r*n+(s-r)*k+q)^(k-1)*binomial(r*u*n+((s-r)*u+t)*k+q*u, n-k)/k!);
for(n=0, M, print1(a(n),", "));

\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * (n+1)^(k-1) * binomial(n-1,n-k)/k!.
b(n) = n!*sum(k=0, n, (-1)^(n-k)*(n+1)^(k-1)*binomial(n-1, n-k)/k!);
for(n=0, 100, print1(a(n)-b(n),", "));

\\ A243954(n) = n! * Sum_{k=0..n-1} (-1)^(n-1-k) * n^(k-1) * binomial(n-2,n-1-k)/k! for n > 0.
a243954(n) = if(n==0, 1, n!*sum(k=0, n-1, (-1)^(n-1-k)*n^(k-1)*binomial(n-2, n-1-k)/k!));
for(n=0, 100, print1(a(n)-a243954(n+1)/(n+1),", "));
