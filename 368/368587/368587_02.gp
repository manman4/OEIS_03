a(n) = n! * sum(k=0, n, (-1)^(n-k) * binomial(k+4,5) / k!);
for(n=0, 22, print1(a(n), ", "));  
