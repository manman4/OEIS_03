M=30;

a(n) = if(n==0, 1, 1 - sum(k=0, n-5, a(k) * a(n-k-5)));
for(n=0, M, print1(a(n), ", "));