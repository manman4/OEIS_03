a(n) = n! * sum(k=0, n, binomial(k+2,3) / k!);
for(n=0, 22, print1(a(n), ", "));  
