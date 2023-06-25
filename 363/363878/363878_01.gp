a(n) = sumdiv(7*n-4, d, d%7==2);

for(n=1, 100, print1(a(n),", "))  

b(n) = sumdiv(7*n-4, d, d%7==5);
for(n=1, 100, print1(a(n)-b(n),", ")) 