\\ a(0) = 1; a(n) = n*a(n-1) + binomial(n+1,2).
a(n) = if(n==0, 1, n*a(n-1) + binomial(n+1,2));
for(n=0, 21, print1(a(n), ", "));  
