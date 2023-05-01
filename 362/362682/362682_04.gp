M=19;

a(n) = if(n==0, 1, -n*a(n-1) + (n+1)^(n-1));
for(n=0, M, print1(a(n), ", "))
