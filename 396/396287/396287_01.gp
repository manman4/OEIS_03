\\ a(n) = a(n-1) + a(n-1)^4 with a(0)=1.
a(n) = if(n==0, 1, a(n-1)+a(n-1)^4);
for(n=0, 7, print1(a(n), ", "));
