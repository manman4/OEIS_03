a(n) = sumdiv(n, d, (n/d)^(d-2)*(d-1));
for(n=1, 47, print1(a(n),", "))  

for(n=1, 47, if(a(n)==n-1, print1(n, ", ")))