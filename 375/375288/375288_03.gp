\\ a(n) = 2*a(n-1) - a(n-2) - 2*a(n-3) - 2*a(n-4) - a(n-6).
a(n) = my(v=[1, 2, 3, 2, -5, -22]); if(n<6, v[n+1], 2*a(n-1) - a(n-2) - 2*a(n-3) - 2*a(n-4) - a(n-6));
for(n=0, 25, print1(a(n),", ")) 