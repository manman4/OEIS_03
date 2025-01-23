\\ a(n) = |Stirling1(n,3)| - Sum_{k=1..n-1} |Stirling1(k,3)| * binomial(n-1,k) * a(n-k).
a(n) = abs(stirling(n,3,1)) - sum(k=1, n-1, abs(stirling(k,3,1)) * binomial(n-1,k) * a(n-k));
for(n=0, 20, print1(a(n), ", "))