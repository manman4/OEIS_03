M=27;

\\ a(n) = Sum_{k=0..floor(n/5)} (-1)^k * (6*k+1) * binomial(2*n-4*k+1,n-5*k)/(2*n-4*k+1).
a(n) = sum(k=0, n\5, (-1)^k * (6*k+1) * binomial(2*n-4*k+1,n-5*k)/(2*n-4*k+1));
for(n=0, M, print1(a(n),", "));  

