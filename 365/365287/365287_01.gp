a(n) = (n!/(n+1)) * sum(k=0, n\3, (n-3*k)^k * binomial(n+1,n-3*k)/k! );
for(n=0, 19, print1(a(n), ", "))

