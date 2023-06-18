a(n) = (n-1)!*sumdiv(n, d, (-1)^(d+1)/(d-1)!);
for(n=1, 23, print1(a(n),", "))     

for(n=1, 100, if(a(n) == 1+(n-1)!, print1(n,", ")))     