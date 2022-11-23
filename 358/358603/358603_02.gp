M=32;

a(n) = if(n<2, 1, (a(n-1) - n * a(n-2) + 1)/2);  
for(n=0, M, print1(a(n), ", "));