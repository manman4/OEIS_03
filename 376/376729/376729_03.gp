\\ a(n) = 2*a(n-2) + 2*a(n-3) - a(n-4) + 2*a(n-5) - a(n-6).
a(n) = my(v=[1, 0, 1, 1, 1, 6]); if(n<6, v[n+1], 2*a(n-2) + 2*a(n-3) - a(n-4) + 2*a(n-5) - a(n-6));
for(n=0, 30, print1(a(n),", "))


