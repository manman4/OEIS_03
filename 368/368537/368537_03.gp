a(n) = if(n<2, n, n * (n^n * (n^6-7*n^4+5*n^3+12*n^2-11*n-6) + 6)/(6 * (n-1)^4));
for(n=0, 17, print1(a(n), ", "));  
