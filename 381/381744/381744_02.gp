M=17;

a006013(n) = binomial(3*n+1,n)/(n+1);
\\ a(n) = Sum_{k=0..2*n} (-1)^k * A006013(k) * A006013(2*n-k).
a(n) = sum(k=0, 2*n, (-1)^k * a006013(k) * a006013(2*n-k));
for(n=0, M, print1(a(n), ", "))
 