M=25;

a(n) = if(n==0, 1, -sum(k=1, n, ispower(k, 4)*a(n-k)));
for(n=0, M, print1(a(n), ", "));