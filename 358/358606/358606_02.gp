M=46;

a(n) = if(n<5, 1, (4 * a(n-1) - n * a(n-5) + 1)/5);  
for(n=0, M, print1(a(n), ", "));