\\ a(n) = (n-1) * (a(n-1) + 5*a(n-2)) for n > 1.
a(n) = if(n<2, 1-n, (n-1)*(a(n-1)+5*a(n-2)));
for(n=0, 20, print1(a(n),", "))