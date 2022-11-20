M=42;

a(n) = if(n<4, 1, (3 * a(n-1) + n * a(n-4) + 1)/4); 
for(n=0, M, print1(a(n), ", "));