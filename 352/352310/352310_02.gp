M=23;

b(n, m) = if(n==0, 1, sum(k=1, n, (-1+(k==m))*binomial(n, k)*b(n-k, m)));
a(n) = b(n, 3);
for(n=0, M, print1(a(n), ", "));

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (-k-1)^(n-3*k)/(6^k*(n-3*k)!). 
c(n) = n! * sum(k=0, n\3, (-k-1)^(n-3*k)/(6^k*(n-3*k)!));
for(n=0, M, print1(a(n)-c(n), ", "));
