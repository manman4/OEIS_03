a(n) = n! * sum(k=0, n\2, (n-2*k)^k * binomial(n-k+1,n-2*k)/( (n-k+1)*k! ) );
for(n=0, 18, print1(a(n), ", "))

