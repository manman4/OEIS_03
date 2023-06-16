a(n) = n!*sumdiv(n, d, (-1)^(d+1)*(n/d)^n/d!);
for(n=1, 14, print1(a(n),", "))     

for(n=1, 100, if(a(n) == (-1)^(n+1) + n^n * n!, print1(n,", ")))     