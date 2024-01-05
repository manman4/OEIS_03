\\ a(0) = 1; a(n) = n*a(n-1) + binomial(n+3,4).
a(n) = if(n==0, 1, n*a(n-1) + binomial(n+3,4));
for(n=0, 21, print1(a(n), ", "));  
