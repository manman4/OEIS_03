\\ a(n) = |Stirling1(n,5)| - Sum_{k=1..n-1} |Stirling1(k,5)| * binomial(n-1,k) * a(n-k).
a(n) = abs(stirling(n,5,1)) - sum(k=1, n-1, abs(stirling(k,5,1)) * binomial(n-1,k) * a(n-k));
for(n=0, 20, print1(a(n), ", "))