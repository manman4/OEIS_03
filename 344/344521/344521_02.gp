M=48;

\\ a(n) = Sum_{k=1..n} phi(k) * binomial(floor(n/k)+2,3). 
a(n) = sum(k=1, n, eulerphi(k) * binomial(n\k+2, 3));
for(n=1, M, print1(a(n), ", "));
