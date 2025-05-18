M=18;

\\ a(n) = (n+1)! * Sum_{k=0..n} (-1)^k * binomial(-n^2,k)/(n+1-k).
a(n) = (n+1)! * sum(k=0, n, (-1)^k * binomial(-n^2, k)/(n+1-k));
for(n=0, M, print1(a(n),", "));


