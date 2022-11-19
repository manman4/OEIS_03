M=22;

a(n) = if(n<7, n!+(n>4), n * a(n-1) + a(n-5) - n * a(n-6));  
for(n=0, M, print1(a(n), ", "));