M=23;

a(n) = if(n<2, 1, a(n-1) - 3*(n-1)*a(n-2));
for(n=0, M, print1(a(n), ", ")); 