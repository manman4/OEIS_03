M=22;

a(n) = if(n<3, 1, (3 * (2*n-1) * a(n-1) - n * a(n-2) + 2 * (n-1) * n * (2*n-3) * a(n-3) + 2 * (2*n-3))/(9 * (n-1)));  
for(n=0, M, print1(a(n), ", "));