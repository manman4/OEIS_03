\\ a(0) = 1; a(n) = n*a(n-1) + (-1)^n * binomial(n+3,4).
a(n) = if(n==0, 1, n*a(n-1) + (-1)^n * binomial(n+3,4));
for(n=0, 22, print1(a(n), ", "));  
