a(n) = n!*sum(k=0, n\2, 3^(n-k)*k!*stirling(n-k, k, 2)/(n-k)!);
for(n=0, 18, print1(a(n), ", "));