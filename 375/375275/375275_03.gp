\\ a(n) = 2*a(n-1) - 3*a(n-2) - 2*a(n-3) - a(n-4).
a(n) = my(v=[1,1,0,-5]); if(n<4, v[n+1], 2*a(n-1) - 3*a(n-2) - 2*a(n-3) - a(n-4));
for(n=0, 25, print1(a(n), ", "))

