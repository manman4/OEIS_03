M=21;

b(n, m) = if(n==0, 1, sum(k=1, n, (-1+(k==m)*m!)*binomial(n, k)*b(n-k, m)));
a(n) = b(n, 4);
for(n=0, M, print1(a(n), ", "));

\\ a(n) = n! * Sum_{k=0..floor(n/4)} (-k-1)^(n-4*k)/(n-4*k)!.
c(n) = n! * sum(k=0, n\4, (-k-1)^(n-4*k)/(n-4*k)!);
for(n=0, M, print1(a(n)-c(n), ", "));
