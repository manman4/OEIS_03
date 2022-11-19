M=22;

a(n) = if(n<5, n!+(n>2), n * a(n-1) + a(n-3) - n * a(n-4));  
for(n=0, M, print1(a(n), ", "));