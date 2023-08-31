a(n) = (n!/(n+1)) * sum(k=0, n\2, (n-2*k)^k * binomial(n+1,n-2*k)/k! );
for(n=0, 18, print1(a(n), ", "))

