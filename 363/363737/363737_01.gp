a(n) = n!*sumdiv(n, d, (-1)^(d+1)/(d!*(n/d)!));
for(n=1, 31, print1(a(n),", "))     

for(n=1, 100, if(a(n) == 2, print1(n,", ")))     