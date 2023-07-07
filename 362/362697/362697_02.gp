b(n) = sumdiv(n, d, (d % 3) == 2);
a(n) = if(n==0, 1, (n-1)! * sum(k=1, n, b(k) * a(n-k)/(n-k)!));
for(n=0, 20, print1(a(n),", "))     