\\ a(n) = (n-1)*(n-2) * (3*a(n-3) + 4*(n-3)*a(n-4)).
a(n) = my(v=[1,0,0,6]); if(n<4, v[n+1], (n-1)*(n-2) * (3*a(n-3) + 4*(n-3)*a(n-4)));
for(n=0, 20, print1(a(n), ", "))
 


