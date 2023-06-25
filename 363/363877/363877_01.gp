a(n) = sumdiv(7*n-2, d, d%7==3);

for(n=1, 100, print1(a(n),", "))  

b(n) = sumdiv(7*n-2, d, d%7==4);
for(n=1, 100, print1(a(n)-b(n),", ")) 