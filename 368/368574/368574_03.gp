\\ a(0) = 0; a(n) = n*a(n-1) + binomial(n+2,3).
a(n) = if(n==0, 0, n*a(n-1) + binomial(n+2,3));
for(n=0, 22, print1(a(n), ", "));  
