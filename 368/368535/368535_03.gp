a(n) = if(n<2, n, n * (6*n^(n+2) - n^5 - 3*n^4 + n^3 + n^2 - 6*n + 2)/(6 * (n-1)^4));
for(n=0, 19, print1(a(n), ", "));  
