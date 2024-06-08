M=26;

\\ a(n) = n! * Sum_{k=0..floor(n/5)} binomial(n-4*k-1,k)/(120^k * (n-5*k)!).
a(n) = n!* sum(k=0, n\5, binomial(n-4*k-1, k)/(120^k * (n-5*k)!));
for(n=0, M, print1(a(n), ", "));
