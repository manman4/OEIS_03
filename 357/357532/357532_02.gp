M=37;

a(n) = if(n<3, 1, (2 * a(n-1) + n * a(n-3) + 1)/3); 
for(n=0, M, print1(a(n), ", "));