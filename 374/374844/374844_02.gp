a(n) = if(n==0, 0, n*a(n-1) + n^n);                     
for(n=0, 25, print1(a(n),", "))
for(n=0, 25, print1(a(n)+n!,", "))