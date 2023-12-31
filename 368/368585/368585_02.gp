a(n) = n! * sum(k=0, n, (-1)^(n-k) * binomial(k+2,3) / k!);
for(n=0, 22, print1(a(n), ", "));  
