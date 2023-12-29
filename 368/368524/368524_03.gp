a(n) = if(n<2, n, n * (n+1) * (n^n - n^2 + n - 1)/(n-1)^3);
for(n=0, 19, print1(a(n), ", "));  
