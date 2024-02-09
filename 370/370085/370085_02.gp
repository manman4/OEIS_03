M=13;

\\ a(0) = 1; a(n) = (n-1)! * Sum_{k=1..n} (2*k)! * a(n-k) / (2^k * (n-k)!).
a(n) = if(n==0, 1, (n-1)! * sum(k=1, n, (2*k)! * a(n-k) / (2^k * (n-k)!)));
for(n=0, M, print1(a(n), ", "))

