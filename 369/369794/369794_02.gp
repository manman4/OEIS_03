\\ Sum_{k=0..floor(6*n/5)} binomial(k,6*n-5*k)
a107025(n) = sum(k=0, (6*n)\5, binomial(k,6*n-5*k));
for(n=0, 50, print1(a107025(n), ", "));
\\ First differences of A107025.
for(n=0, 34, print1(a107025(n)-a107025(n-1), ", "));