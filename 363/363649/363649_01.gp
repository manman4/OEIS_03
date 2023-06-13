a(n) = sumdiv(n, d, (n/d)^(n-2*n/d)*(d-1));
for(n=1, 33, print1(a(n),", "))  

for(n=1, 43, if(a(n)==n-1, print1(n, ", ")))