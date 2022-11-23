M=22;

a(n) = if(n<5, n!+(n>3), n * a(n-1) + a(n-4) - n * a(n-5));  
for(n=0, M, print1(a(n), ", "));